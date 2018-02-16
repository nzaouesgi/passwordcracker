#include <gtk/gtk.h>
#include <string.h>
#include "logs.h"
#include "../threads/threads.h"
#include "../struct/gtkstruct.h"
#include "../interface/interface.h"

int getRow (void * row, int argc, char ** argv, char ** colName){

  struct LogEntry * castedRow = (struct LogEntry *) row;
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
  return 0;
}

int fillListStore(GtkListStore * list, struct MainWindow * windowHandler){
  sqlite3 * db;
  int i;
  int y;
  struct LogEntry row;
  int * result = malloc(sizeof(int));
  int res2;
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
    return 0;
  }


  for (i = 0; i < *result;i++){
    sprintf(sql2,"SELECT * FROM logs WHERE attackId = %d;",i);
    check = sqlite3_exec(db, sql2, getRow, (void *)&row, &errMsg);

    if (check != 0){
      printf("Something wrong happened with opening the database.\n");
      sqlite3_close(db);
      return 0;
    }

    gtk_list_store_insert_with_values(windowHandler->logsWindow->model,NULL,-1,
      0,row.attackId,
      1,row.time,
      2,row.host,
      3,row.port,
      4,row.method,
      5,row.success,
      6,row.pwdDump,-1);
  }

  res2 = *result;
  free(result);

  sqlite3_close(db);
  return res2;
}

void viewLogs (GtkButton *button,
               gpointer   user_data){

	struct MainWindow * windowHandler = (struct MainWindow *)user_data;

  GtkTreeViewColumn * column;
  GtkCellRenderer * renderer = gtk_cell_renderer_text_new();

	windowHandler->logsWindow->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  windowHandler->logsWindow->scrolledWindow = gtk_scrolled_window_new(NULL,NULL);
  windowHandler->logsWindow->logsTreeView = gtk_tree_view_new();
  gtk_container_add(GTK_CONTAINER(windowHandler->logsWindow->scrolledWindow), windowHandler->logsWindow->logsTreeView);
  windowHandler->logsWindow->model = gtk_list_store_new(
    7,
    G_TYPE_STRING,
    G_TYPE_STRING,
    G_TYPE_STRING,
    G_TYPE_STRING,
    G_TYPE_STRING,
    G_TYPE_STRING,
    G_TYPE_STRING
  );

  fillListStore(windowHandler->logsWindow->model, windowHandler);

  gtk_tree_view_set_model(GTK_TREE_VIEW(windowHandler->logsWindow->logsTreeView),
    GTK_TREE_MODEL(windowHandler->logsWindow->model));

  column = gtk_tree_view_column_new_with_attributes("ID",renderer,"text",0,NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(windowHandler->logsWindow->logsTreeView),column);
  column = gtk_tree_view_column_new_with_attributes("Date",renderer,"text",1,NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(windowHandler->logsWindow->logsTreeView),column);
  column = gtk_tree_view_column_new_with_attributes("Host",renderer,"text",2,NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(windowHandler->logsWindow->logsTreeView),column);
  column = gtk_tree_view_column_new_with_attributes("Port",renderer,"text",3,NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(windowHandler->logsWindow->logsTreeView),column);
  column = gtk_tree_view_column_new_with_attributes("Method",renderer,"text",4,NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(windowHandler->logsWindow->logsTreeView),column);
  column = gtk_tree_view_column_new_with_attributes("Result",renderer,"text",5,NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(windowHandler->logsWindow->logsTreeView),column);
  column = gtk_tree_view_column_new_with_attributes("Password",renderer,"text",6,NULL);
  gtk_tree_view_append_column(GTK_TREE_VIEW(windowHandler->logsWindow->logsTreeView),column);

  gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(windowHandler->logsWindow->logsTreeView),TRUE);

  gchar * text = "<b>User logs</b>";
  windowHandler->logsWindow->label= gtk_label_new(g_locale_to_utf8(text,-1,NULL,NULL,NULL));
  gtk_label_set_justify(GTK_LABEL(windowHandler->logsWindow->label),GTK_JUSTIFY_CENTER);
  gtk_label_set_use_markup(GTK_LABEL(windowHandler->logsWindow->label),TRUE);

	gtk_window_set_title(GTK_WINDOW(windowHandler->logsWindow->window), "Attack logs");
  gtk_container_set_border_width(GTK_CONTAINER(windowHandler->logsWindow->window), 3);
  gtk_window_set_position(GTK_WINDOW(windowHandler->logsWindow->window), GTK_WIN_POS_CENTER);

  windowHandler->logsWindow->box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);

  gtk_container_add(GTK_CONTAINER(windowHandler->logsWindow->window), windowHandler->logsWindow->box);

  gtk_box_pack_start(GTK_BOX(windowHandler->logsWindow->box),windowHandler->logsWindow->label,FALSE,FALSE,0);
  gtk_box_pack_start(GTK_BOX(windowHandler->logsWindow->box),windowHandler->logsWindow->scrolledWindow,TRUE,TRUE,0);
  gtk_widget_set_size_request(windowHandler->logsWindow->scrolledWindow, 1000, 250);

  gtk_widget_show_all(windowHandler->logsWindow->window);
}
