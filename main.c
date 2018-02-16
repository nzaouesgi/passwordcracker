#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include <pthread.h>
#include <gtk/gtk.h>
#include <unistd.h>
#include <time.h>
#include <libssh/libssh.h>
#include <libssh/callbacks.h>
#include <curl/curl.h>
#include "auth/sqlauth.h"
#include "struct/gtkstruct.h"

// cette ligne sert à dire au compilateur que la librairie sqlite possède
// les codecs de chiffrage pour la BDD.

#define SQLITE_TEMP_STORE 2

// cette ligne est liée à pthread sert à activer la réentrance
//(le fait pour une fonction d'être utilisable par
//plusieurs processus en même temps dans le cadre du multi-threading).

#define D_REENTRANT

int main (int argc, char ** argv){

  // On commence par vérifier l'existence de la bdd.

  sqlite3 * db; // pointeur représentant la connection à la bdd
  char sql [1000]; // la requête SQL
  int rc;
  int loginOn = 0;
  // pointeurs vers la structure principale du programme, ou vers la structure de la fenêtre de création de DB.

  // partie fenêtre de l'interface principal (on l'appelle loginWin car au début, la fenêtre de login,
  // qui se transforme ensuite en interface principale, dans les fonctions post authentification,
  // elle porte le nom de windowHandler ou handler).
  struct MainWindow * loginWin;
  // partie fenêtre de création de DB. ne se transforme pas au niveau de l'interface.
  struct DbWindow * dbWindow;
  // initialisation des structures de données de libssh et de libcurl.
  ssh_threads_set_callbacks(ssh_threads_get_pthread());
  ssh_init();
  curl_global_init(CURL_GLOBAL_ALL);
  // initialisation de l'espace-temps.
  srand(time(NULL));
  // initialisation de GTK.
  gtk_init(&argc, &argv);
  // Ouverture de la BDD, seulement si le fihier de base existe.
  rc = sqlite3_open_v2("sqlite/data.db",&db,SQLITE_OPEN_READWRITE,NULL);

  if (rc != SQLITE_OK){
    // le fichier n'existe pas. On va alors créer une nouvelle bdd chiffrée.
    dbWindow = malloc(sizeof(*dbWindow));
    createDBWindow(dbWindow);
  }

  else {
    // Le fichier existe. Début de la phase d'authentification.

    /* On initialise les widgets sous forme d'une structure pour tout le menu.
    Cela permet d'envoyer à la fonction de callback l'ensemble de la fenêtre principale
    pour pourvoir la transformer lorsque le login est réussi.*/

    loginOn = 1;
    //allocation en mémoire de la structure principale + structure de la fenêtre de logs.
    loginWin = malloc(sizeof(*loginWin));
    loginWin->logsWindow = malloc(sizeof(*(loginWin->logsWindow)));
    // création de la fenêtre principale.
    loginWin->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    loginWin->box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    loginWin->button = gtk_button_new_with_label("Enter");

    loginWin->input = gtk_entry_new();
    gchar * text= "<b>Please enter your password:</b>\n";
    loginWin->label= gtk_label_new (g_locale_to_utf8(text,-1,NULL,NULL,NULL));

    // ajout du conteneur principal (GtkBox)
    gtk_container_add(GTK_CONTAINER(loginWin->window), loginWin->box);

    // configuration des GtkEntry.
    gtk_entry_set_max_length(GTK_ENTRY(loginWin->input), 30);
    gtk_entry_set_placeholder_text(GTK_ENTRY(loginWin->input), "Password");
    gtk_entry_set_visibility(GTK_ENTRY(loginWin->input),FALSE);
    gtk_entry_set_invisible_char(GTK_ENTRY(loginWin->input),'x');

    // configuration du GtkLabel.
    gtk_label_set_justify(GTK_LABEL(loginWin->label),GTK_JUSTIFY_CENTER);
    gtk_label_set_use_markup(GTK_LABEL(loginWin->label),TRUE);

    // configuration de la GtkWindow.
    gtk_window_set_title(GTK_WINDOW(loginWin->window), "Welcome back...");
    gtk_window_set_resizable(GTK_WINDOW(loginWin->window), FALSE);
    gtk_widget_set_size_request(loginWin->window,200 , 150);
    gtk_container_set_border_width(GTK_CONTAINER(loginWin->window), 10);
    gtk_window_set_position(GTK_WINDOW(loginWin->window), GTK_WIN_POS_CENTER);

    // ajout des widgets dans le conteneur.
    gtk_box_pack_start(GTK_BOX(loginWin->box),loginWin->label,FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(loginWin->box),loginWin->input,FALSE,FALSE,0);
    gtk_box_pack_start(GTK_BOX(loginWin->box), loginWin->button,FALSE,FALSE,10);

    // apparition des widgets.
    gtk_widget_show_all(loginWin->window);

    // connection des signaux de fermeture de fenêtre, et de clic sur le bouton.
    g_signal_connect(G_OBJECT(loginWin->button), "clicked", G_CALLBACK(checkPassword), loginWin);
    g_signal_connect(G_OBJECT(loginWin->window), "destroy", G_CALLBACK(gtk_main_quit), NULL);
  }
  // Boucle principale de GTK. C'est ici que le main boucle jusqu'à la fin du programme.
  // tout ce qui se passe dans le programme n'est que l'exécution des fonctions de callbacks
  // liée aux signaux GTK.
  gtk_main();
  // si on avait alloué la structure de fenêtre du programme principal, on la libère.
  if (loginOn == 1){
    free(loginWin->logsWindow);
    free(loginWin);
  } else {
    free(dbWindow);
  }
  // nettoyage de libcurl et libssh.
  curl_global_cleanup();
  ssh_finalize();
  // fin du programme.
  exit(EXIT_SUCCESS);
}
