#include "interface.h"

// sert à écrire dans la console + scroll down.

void writeConsole (struct MainWindow * windowHandler, const gchar * text){

  // écriture du message.
  GtkTextIter iter;
  gtk_text_buffer_get_end_iter(windowHandler->consoleBuffer, &iter);
  gtk_text_buffer_insert(windowHandler->consoleBuffer,&iter,text,strlen(text));

}

// Fonctions de callback servant à l'écriture basique dans la console.
/* USAGE:

CallbackMessage * msg = malloc(sizeof(*msg));
msg->handle = attackStackPtr->handler;
strcpy(msg->message,"mon message");
g_idle_add_full(100,writeConsoleCallback,(gpointer)msg,NULL);

*/

gboolean writeConsoleCallback (gpointer user_data){

  struct CallbackMessage * msg = (struct CallbackMessage *) user_data;
  struct MainWindow * windowHandler = (struct MainWindow *) msg->handle;

  GtkTextIter iter;
  gtk_text_buffer_get_end_iter(windowHandler->consoleBuffer, &iter);
  gtk_text_buffer_insert(windowHandler->consoleBuffer,&iter,msg->message ,strlen(msg->message));

  free(msg);
  return FALSE;
}

/* USAGE:
CallbackMessage * msg = malloc(sizeof(*msg));
msg->handle = attackStackPtr->handler;
strcpy(msg->message,"message erreur");
strcpy(msg->tagname,"error_console");

g_idle_add_full(100,writeConsoleCallback,(gpointer)msg,NULL);
*/

gboolean writeConsoleWithTagCallback (gpointer user_data){

  struct CallbackMessage * msg = (struct CallbackMessage *) user_data;
  struct MainWindow * windowHandler = (struct MainWindow *) msg->handle;

  GtkTextIter iter;
  gtk_text_buffer_get_end_iter(windowHandler->consoleBuffer, &iter);
  gtk_text_buffer_insert_with_tags_by_name(windowHandler->consoleBuffer,&iter,msg->message ,strlen(msg->message)
  ,msg->tagname,NULL);

  free(msg);
  return FALSE;
}

void writeConsoleWithTag (struct MainWindow * windowHandler, const gchar * text,const gchar * tagname){

  // écriture du message.
  GtkTextIter iter;
  gtk_text_buffer_get_end_iter(windowHandler->consoleBuffer, &iter);
  gtk_text_buffer_insert_with_tags_by_name(windowHandler->consoleBuffer,&iter,text,strlen(text), tagname,NULL);

}

void scrollDown (GtkTextBuffer * textbuffer, gpointer user_data) {

  struct MainWindow * windowHandler = (MainWindow *) user_data;

  GtkTextIter iter;
  gtk_text_buffer_get_end_iter(windowHandler->consoleBuffer, &iter);

  GtkTextMark * mark = gtk_text_mark_new(NULL,FALSE);
  gtk_text_buffer_add_mark(textbuffer,mark,&iter);
  gtk_text_view_scroll_to_mark (GTK_TEXT_VIEW(windowHandler->console), mark, 0, FALSE,0,0);
  gtk_text_buffer_delete_mark(textbuffer,mark);
}
