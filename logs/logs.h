#include "../struct/gtkstruct.h"

#ifndef LOGS_H
#define LOGS_H

void viewLogs (GtkButton *button,
               gpointer   user_data);
int fillListStore(GtkListStore * list, struct MainWindow * windowHandler);
int  getRow (void * row, int argc, char ** argv, char ** colName);

#endif
