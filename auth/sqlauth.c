#include "../struct/gtkstruct.h"
#include "sqlauth.h"
#include "../interface/interface.h"

void checkPassword (GtkButton * button, gpointer user_data){

  struct MainWindow * loginWin =  (struct MainWindow *) user_data;
  const gchar * input1 = gtk_entry_get_text(GTK_ENTRY(loginWin->input));
  sqlite3 * db;
  char  sql [1000];
  int check;
  char *errMsg;

  // ouverture de la bdd.
  check = sqlite3_open("sqlite/data.db", &db);

  if (check != 0){
    printf("Something wrong happened with opening the database.\n");
    sqlite3_close(db);
    gtk_main_quit();
  }
  // on teste la clé.
  sqlite3_key(db, input1, strlen(input1));
  sprintf(sql,"SELECT count(*) FROM logs;");
  check = sqlite3_exec(db, sql, NULL, 0, &errMsg);
  // erreur d'authentification.
  if( check != SQLITE_OK ){
    GtkWidget *dialog;
    dialog = gtk_message_dialog_new(GTK_WINDOW(loginWin->window),
          GTK_DIALOG_DESTROY_WITH_PARENT,
          GTK_MESSAGE_WARNING,
          GTK_BUTTONS_OK,
          "Bad password.");
    gtk_window_set_title(GTK_WINDOW(dialog), "Error");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    sqlite3_free(errMsg);
  }
  else {
    strcpy(loginWin->dbPassword,input1);
    gtk_widget_destroy(loginWin->button);
    gtk_widget_destroy(loginWin->input);
    gtk_widget_destroy(loginWin->label);
    gtk_widget_destroy(loginWin->box);
    gtk_widget_hide(loginWin->window);

    startApp(loginWin);
  }
  sqlite3_close(db);
}

void checkPasswords (GtkButton * button, gpointer user_data){

  struct DbWindow * dbWindow = (struct DbWindow *) user_data;
  sqlite3 * db;
  char  * sql;
  int check;
  char *errMsg;

  const gchar * input1 = gtk_entry_get_text(GTK_ENTRY(dbWindow->inputs[0]));
  const gchar * input2 = gtk_entry_get_text(GTK_ENTRY(dbWindow->inputs[1]));

  if (strcmp(input1,input2) == 0 && strlen(input1) >= 4){

    // ouverture d'une nouvelle bdd.
    check = sqlite3_open("sqlite/data.db", &db);

    if (check != 0){
      printf("Something wrong happened with opening the database.\n");
      sqlite3_close(db);
      gtk_main_quit();
    }

    //chiffrage.
    sqlite3_key(db, input1, strlen(input1) );

    // création de la table de logs.
    sql = "CREATE TABLE logs \
      (attackId INT PRIMARY KEY,\
       time DATETIME DEFAULT CURRENT_TIMESTAMP,\
       host VARCHAR (100) NOT NULL,\
       port VARCHAR(100) NOT NULL,\
       method VARCHAR(100) NOT NULL,\
       success INT DEFAULT 0,\
       pwdDump VARCHAR (100))";
    check = sqlite3_exec(db, sql, NULL, 0, &errMsg);

    if( check != SQLITE_OK ){
       sqlite3_free(errMsg);
       sqlite3_close(db);
       gtk_main_quit();
    } else {

      // succès. --> redémarrage application.
      GtkWidget * dialog;
      dialog = gtk_message_dialog_new(GTK_WINDOW(dbWindow->window),
            GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_INFO,
            GTK_BUTTONS_OK,
            "Database created successfully.\nPlease restart the application.");
      gtk_window_set_title(GTK_WINDOW(dialog), "Just one more step...");
      gtk_dialog_run(GTK_DIALOG(dialog));
      gtk_widget_destroy(dialog);
      sqlite3_close(db);
      gtk_main_quit();
    }

  }

    // message d'erreur.
  else {
    GtkWidget * dialog;
    dialog = gtk_message_dialog_new(GTK_WINDOW(dbWindow->window),
          GTK_DIALOG_DESTROY_WITH_PARENT,
          GTK_MESSAGE_ERROR,
          GTK_BUTTONS_OK,
          "Passwords must match, and be at least 4 digits.\nPlease try again.");
    gtk_window_set_title(GTK_WINDOW(dialog), "Error");
    gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
  }
}

// En cas d'absence de bdd, cette fenêtre sert à en créer une nouvelle.

void createDBWindow (struct DbWindow * dbWindow) {

  /* les widgets sont initialisés sous forme de struture pour
   pouvoir les envoyer à la fonction de callback plus simplement.*/

  dbWindow->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  dbWindow->box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
  dbWindow->button = gtk_button_new_with_label("Enter");
  dbWindow->inputs[0] = gtk_entry_new();
  dbWindow->inputs[1] = gtk_entry_new();
  gchar * text = "<b>No database found.</b>\n \
  Please enter a password for a new database: ";
  dbWindow->label= gtk_label_new(g_locale_to_utf8(text,-1,NULL,NULL,NULL));

  // ajout du conteneur principal (GtkBox)
  gtk_container_add(GTK_CONTAINER(dbWindow->window), dbWindow->box);

  // configuration des GtkEntry.
  gtk_entry_set_max_length(GTK_ENTRY(dbWindow->inputs[0]), 30);
  gtk_entry_set_max_length(GTK_ENTRY(dbWindow->inputs[1]), 30);
  gtk_entry_set_placeholder_text(GTK_ENTRY(dbWindow->inputs[0]), "Password");
  gtk_entry_set_placeholder_text(GTK_ENTRY(dbWindow->inputs[1]), "Confirmation");
  gtk_entry_set_visibility(GTK_ENTRY(dbWindow->inputs[0]),FALSE);
  gtk_entry_set_visibility(GTK_ENTRY(dbWindow->inputs[1]),FALSE);
  gtk_entry_set_invisible_char(GTK_ENTRY(dbWindow->inputs[0]),'x');
  gtk_entry_set_invisible_char(GTK_ENTRY(dbWindow->inputs[1]),'x');

  // configureation du GtkLabel.
  gtk_label_set_justify(GTK_LABEL(dbWindow->label),GTK_JUSTIFY_CENTER);
  gtk_label_set_use_markup(GTK_LABEL(dbWindow->label),TRUE); // utilisation des markups = ON

  // configuration de la GtkWindow.
  gtk_window_set_title(GTK_WINDOW(dbWindow->window), "Welcome");
  gtk_window_set_resizable(GTK_WINDOW(dbWindow->window), FALSE);
  gtk_widget_set_size_request(dbWindow->window,200 , 200);
  gtk_container_set_border_width(GTK_CONTAINER(dbWindow->window), 15);
  gtk_window_set_position(GTK_WINDOW(dbWindow->window), GTK_WIN_POS_CENTER);

  // ajout des widgets dans le conteneur GtkBox.
  gtk_box_pack_start(GTK_BOX(dbWindow->box),dbWindow->label,FALSE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(dbWindow->box),dbWindow->inputs[0],FALSE,FALSE,10);
  gtk_box_pack_start(GTK_BOX(dbWindow->box),dbWindow->inputs[1],FALSE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(dbWindow->box), dbWindow->button,FALSE,FALSE,10);

  // apparition des widgets.
  gtk_widget_show(dbWindow->window);
  gtk_widget_show(dbWindow->box);
  gtk_widget_show(dbWindow->label);
  gtk_widget_show(dbWindow->inputs[0]);
  gtk_widget_show(dbWindow->inputs[1]);
  gtk_widget_show(dbWindow->button);

  // connection des signaux de fermeture de fenêtre, et de clic sur le bouton.
  g_signal_connect(G_OBJECT(dbWindow->button), "clicked", G_CALLBACK(checkPasswords), dbWindow);
  g_signal_connect(G_OBJECT(dbWindow->window), "destroy", G_CALLBACK(gtk_main_quit), NULL);

}
