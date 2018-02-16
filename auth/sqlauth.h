#ifndef SQLAUTH_H
#define SQLAUTH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <gtk/gtk.h>
#include <sqlite3.h>
#include "../struct/gtkstruct.h"

void checkPasswords (GtkButton * button , gpointer user_data);
void checkPassword (GtkButton * button , gpointer user_data);
void createDBWindow (struct DbWindow * dbWindow);

#endif
