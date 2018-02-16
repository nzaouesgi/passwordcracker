#include <gtk/gtk.h>
#include <string.h>
#include "../logs/logs.h"
#include "export.h"
#include "../threads/threads.h"
#include "../struct/gtkstruct.h"
#include "../interface/interface.h"

int saveRow (void * row, int argc, char ** argv, char ** colName){

  struct LogEntry * castedRow = (struct LogEntry *) row;
  int i;

  strcpy(castedRow->attackId,argv[0]);
  strcpy(castedRow->time,argv[1]);
  strcpy(castedRow->host,argv[2]);
  strcpy(castedRow->port,argv[3]);
  strcpy(castedRow->method,argv[4]);
  strcpy(castedRow->success,argv[5]);
  if (strcmp(castedRow->success,"1") == 0){
    strcpy(castedRow->success,"Success");
  } else {
    strcpy(castedRow->success,"Failure");
  }
  if (argv[6] != NULL){
    strcpy(castedRow->pwdDump,argv[6]);
  } else {
    strcpy(castedRow->pwdDump,"?");
  }

  fprintf(castedRow->exportFile,"Log entry #%s\n\
Time: %s\n\
Host: %s\n\
Port: %s\n\
Method: %s\n\
Success: %s\n\
Password dump: %s\n\n",castedRow->attackId,
    castedRow->time,
    castedRow->host,
    castedRow->port,
    castedRow->method,
    castedRow->success,
    castedRow->pwdDump);

  return 0;
}

void exportLogs (GtkButton *button, gpointer   user_data){

    struct MainWindow * windowHandler = (struct MainWindow *)user_data;

    GtkWidget *dialog;
    GtkFileChooserAction action = GTK_FILE_CHOOSER_ACTION_SAVE;
    gint res;

    dialog = gtk_file_chooser_dialog_new ("Export logs",GTK_WINDOW(windowHandler->window),
    action,"Cancel",GTK_RESPONSE_CANCEL,"Save",GTK_RESPONSE_ACCEPT,NULL);

    gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER(dialog), TRUE);

    res = gtk_dialog_run (GTK_DIALOG (dialog));
    if (res == GTK_RESPONSE_ACCEPT){

      FILE * output;
      windowHandler->filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));

      output = fopen(windowHandler->filename,"wb");

      sqlite3 * db;
      int i;
      int y;
      struct LogEntry row;
      row.exportFile = output;
      int * result = malloc(sizeof(int));
      char  sql [1000];
      char  sql2 [1000];
      int check;
      char *errMsg;
      char tmpDbPasswd [255];
      strcpy(tmpDbPasswd,windowHandler->dbPassword);

      // ouverture de la bdd.
      check = sqlite3_open("sqlite/data.db", &db);

      sqlite3_key(db, tmpDbPasswd, strlen(tmpDbPasswd));

      sprintf(sql,"SELECT count(*) FROM logs;");
      check = sqlite3_exec(db, sql, countLogs, (void *)result, &errMsg);
      if (check != 0){
        printf("Something wrong happened with opening the database.\n");
        sqlite3_close(db);
      }


      for (i = 0; i < *result;i++){
        sprintf(sql2,"SELECT * FROM logs WHERE attackId = %d;",i);
        check = sqlite3_exec(db, sql2, saveRow, (void *)&row, &errMsg);

        if (check != 0){
          printf("Something wrong happened with fetching from the database.\n");
          sqlite3_close(db);
        }
      }

      free(result);
      fclose(output);

      sqlite3_close(db);
    }

    gtk_widget_destroy(dialog);

  }
