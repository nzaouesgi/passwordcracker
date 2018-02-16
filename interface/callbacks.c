#include "interface.h"
#include "../attack/attack.h"

// sert à ouvrir la recherche de fichier et enregistrer le nom du fichier
// dans le label fileInputLabel.

void openFile (GtkButton * button, gpointer user_data){

  struct MainWindow * windowHandler = (struct MainWindow *) user_data;

  GtkWidget *dialog;
  GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
  gint res;

  dialog = gtk_file_chooser_dialog_new ("Open File",GTK_WINDOW(windowHandler->window),
  action,"Cancel",GTK_RESPONSE_CANCEL,"Open",GTK_RESPONSE_ACCEPT,NULL);

  res = gtk_dialog_run (GTK_DIALOG (dialog));
  if (res == GTK_RESPONSE_ACCEPT)
  {
    int i;
    int count = 0;
    int seek = 0;
    long size;
    FILE * fp;

    windowHandler->filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
    for (i = 0; i < strlen(windowHandler->filename); i ++){
      count++;
      if (windowHandler->filename[i] == '/'){
        count = 0;
      }
    }

    fp = fopen(windowHandler->filename,"r");
    seek = fseeko(fp,0,SEEK_END);
    size = ftello(fp);

    if ( fp != NULL && seek == 0 && size > 0 ){
    gtk_label_set_text(GTK_LABEL(windowHandler->fileInputLabel),
    windowHandler->filename+strlen(windowHandler->filename)-count);
    writeConsoleWithTag(windowHandler,"[METHOD] ","strong_console");
    writeConsole(windowHandler,"Dictionnary file set to: ");
    writeConsole(windowHandler,(const char *)windowHandler->filename);
    writeConsole(windowHandler,"\n");
    fclose(fp);
    }

    else {
      writeConsoleWithTag(windowHandler,"[","strong_console");
      writeConsoleWithTag(windowHandler,"ERROR","error_console");
      writeConsoleWithTag(windowHandler,"] ","strong_console");
      writeConsole(windowHandler,"Not a valid file, may be empty. Please try again.\n");
    }
  }
  gtk_widget_destroy (dialog);
}

/* Cette fonction sert à désactiver la partie attaque dictionnaire
lorsqu'on clique sur le radio button bruteforce, aussi on va update
les variables de contexte propres à notre structure principale.*/

void turnOffDicPart (GtkToggleButton * toggledbutton, gpointer user_data){

  if (gtk_toggle_button_get_active(toggledbutton) == TRUE ){

  struct MainWindow * windowHandler = (struct MainWindow *) user_data;

  windowHandler->optMethodBrute = 1;
  windowHandler->optMethodDic = 0;

  writeConsoleWithTag(windowHandler,"[METHOD] ","strong_console");
  writeConsole(windowHandler,"Method set to: bruteforce attack\n");

  gtk_widget_set_sensitive(windowHandler->fileButton,FALSE);

  gtk_widget_set_sensitive(windowHandler->bruteLengthStart,TRUE);
  gtk_widget_set_sensitive(windowHandler->bruteLengthEnd,TRUE);
  gtk_widget_set_sensitive(windowHandler->bruteUpperCase,TRUE);
  gtk_widget_set_sensitive(windowHandler->bruteNumbers,TRUE);
  gtk_widget_set_sensitive(windowHandler->bruteSpecialChars,TRUE);

}

}

// Cette fonction fait le même travail que la précendente, en inversé.

void turnOffBrutePart (GtkToggleButton * toggledbutton, gpointer user_data){

  if (gtk_toggle_button_get_active(toggledbutton) == TRUE ){

  struct MainWindow * windowHandler = (struct MainWindow *) user_data;

  windowHandler->optMethodDic = 1;
  windowHandler->optMethodBrute = 0;

  writeConsoleWithTag(windowHandler,"[METHOD] ","strong_console");
  writeConsole(windowHandler,"Method set to: dictionnary attack\n");

  gtk_widget_set_sensitive(windowHandler->bruteLengthStart,FALSE);
  gtk_widget_set_sensitive(windowHandler->bruteLengthEnd,FALSE);
  gtk_widget_set_sensitive(windowHandler->bruteUpperCase,FALSE);
  gtk_widget_set_sensitive(windowHandler->bruteNumbers,FALSE);
  gtk_widget_set_sensitive(windowHandler->bruteSpecialChars,FALSE);

  gtk_widget_set_sensitive(windowHandler->fileButton,TRUE);

}

}

// fonctions servant à update la valeur des variables de contexte GUI
// lorsque les checkboxes sont cliquées.

void setUpperCase (GtkButton * button, gpointer user_data){

  struct MainWindow * windowHandler = (struct MainWindow *) user_data;

  if (windowHandler->optMethodBruteUppercase == 0){
    windowHandler->optMethodBruteUppercase = 1;
    writeConsoleWithTag(windowHandler,"[BRUTEFORCE] ","strong_console");
    writeConsole(windowHandler,"Uppercase: ON\n");
  } else {
    windowHandler->optMethodBruteUppercase = 0;
    writeConsoleWithTag(windowHandler,"[BRUTEFORCE] ","strong_console");
    writeConsole(windowHandler,"Uppercase: OFF\n");
  }
}

void setNumbers (GtkButton * button, gpointer user_data){

  struct MainWindow * windowHandler = (struct MainWindow *) user_data;

  if (windowHandler->optMethodBruteNumbers == 0){
    windowHandler->optMethodBruteNumbers = 1;
    writeConsoleWithTag(windowHandler,"[BRUTEFORCE] ","strong_console");
    writeConsole(windowHandler,"Numbers: ON\n");
  } else {
    windowHandler->optMethodBruteNumbers = 0;
    writeConsoleWithTag(windowHandler,"[BRUTEFORCE] ","strong_console");
    writeConsole(windowHandler,"Numbers: OFF\n");
  }
}

void setSpecialChars (GtkButton * button, gpointer user_data){

  struct MainWindow * windowHandler = (struct MainWindow *) user_data;

  if (windowHandler->optMethodBruteSpecialChars == 0){
    windowHandler->optMethodBruteSpecialChars = 1;
    writeConsoleWithTag(windowHandler,"[BRUTEFORCE] ","strong_console");
    writeConsole(windowHandler,"Special characters: ON\n");
  } else {
    windowHandler->optMethodBruteSpecialChars = 0;
    writeConsoleWithTag(windowHandler,"[BRUTEFORCE] ","strong_console");
    writeConsole(windowHandler,"Special characters: OFF\n");
  }
}

// désactiver le GUI lorsque l'attaque est en cours.

void disableButtons (GtkButton * button, gpointer user_data){

  struct MainWindow * windowHandler = (struct MainWindow *) user_data;

  gtk_widget_set_sensitive(windowHandler->protoList ,FALSE);
  gtk_widget_set_sensitive(windowHandler->hostInput ,FALSE);
  gtk_widget_set_sensitive(windowHandler->portInput ,FALSE);
  gtk_widget_set_sensitive(windowHandler->threadsScale ,FALSE);

  gtk_widget_set_sensitive(windowHandler->dicRadio ,FALSE);
  gtk_widget_set_sensitive(windowHandler->fileButton ,FALSE);
  gtk_widget_set_sensitive(windowHandler->bruteRadio ,FALSE);
  gtk_widget_set_sensitive(windowHandler->bruteLengthStart ,FALSE);
  gtk_widget_set_sensitive(windowHandler->bruteLengthEnd ,FALSE);
  gtk_widget_set_sensitive(windowHandler->bruteUpperCase ,FALSE);
  gtk_widget_set_sensitive(windowHandler->bruteNumbers ,FALSE);
  gtk_widget_set_sensitive(windowHandler->bruteSpecialChars ,FALSE);

  gtk_widget_set_sensitive(windowHandler->startButton ,FALSE);

  gtk_widget_set_sensitive(windowHandler->stopButton ,TRUE);

  // lancement de l'attaque

  startAttack(windowHandler);

}

// re-activer le GUI lorsque l'attaque est stoppée/terminée
//+ donner le signal d'arret au processus principal de craking, et attendre sa terminaison.

void enableButtons (GtkButton * button, gpointer user_data){

  struct MainWindow * windowHandler = (struct MainWindow *) user_data;

  if (windowHandler->stopped == 0){
    windowHandler->stopped = 1;
    pthread_join(windowHandler->mainThread,NULL);
  }

  gtk_widget_set_sensitive(windowHandler->protoList ,TRUE);
  gtk_widget_set_sensitive(windowHandler->hostInput ,TRUE);
  gtk_widget_set_sensitive(windowHandler->portInput ,TRUE);
  gtk_widget_set_sensitive(windowHandler->threadsScale ,TRUE);
  gtk_widget_set_sensitive(windowHandler->bruteRadio ,TRUE);
  gtk_widget_set_sensitive(windowHandler->dicRadio ,TRUE);

  if (windowHandler->optMethodBrute == 1){
    gtk_widget_set_sensitive(windowHandler->bruteLengthStart ,TRUE);
    gtk_widget_set_sensitive(windowHandler->bruteLengthEnd ,TRUE);
    gtk_widget_set_sensitive(windowHandler->bruteUpperCase ,TRUE);
    gtk_widget_set_sensitive(windowHandler->bruteNumbers ,TRUE);
    gtk_widget_set_sensitive(windowHandler->bruteSpecialChars ,TRUE);
  } else {
    gtk_widget_set_sensitive(windowHandler->fileButton ,TRUE);
  }

  gtk_widget_set_sensitive(windowHandler->startButton ,TRUE);

  gtk_widget_set_sensitive(windowHandler->stopButton ,FALSE);

  stopAttack(windowHandler) ;

}

gboolean endAttack (gpointer user_data){
  struct MainWindow * windowHandler = (struct MainWindow *) user_data;
  g_signal_emit_by_name(G_OBJECT(windowHandler->stopButton),"clicked");
  return FALSE;
}
