#ifndef INTERFACE_H
#define INTERFACE_H

#include "../auth/sqlauth.h"
#include "../struct/gtkstruct.h"

// le nombre maximum de threads que l'utilisateur peut fixer.
#define MAX_THREADS 50

void startApp(struct MainWindow * windowHandler);
gboolean endAttack (gpointer user_data);
void disableButtons (GtkButton * button, gpointer user_data);
void enableButtons (GtkButton * button, gpointer user_data);
void openFile (GtkButton * button, gpointer user_data);
void writeConsole (struct MainWindow * windowHandler, const gchar * text);
void writeConsoleWithTag (struct MainWindow * windowHandler, const gchar * text,const gchar * tagname);
gboolean  writeConsoleCallback (gpointer data);
gboolean  writeConsoleWithTagCallback (gpointer data);
void scrollDown(GtkTextBuffer * textbuffer, gpointer user_data);
void turnOffDicPart (GtkToggleButton * toggledbutton, gpointer user_data);
void turnOffBrutePart (GtkToggleButton * toggledbutton, gpointer user_data);
void setUpperCase (GtkButton * button, gpointer user_data);
void setNumbers (GtkButton * button, gpointer user_data);
void setSpecialChars(GtkButton * button, gpointer user_data);

#endif
