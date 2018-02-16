#include "../struct/gtkstruct.h"
#include <string.h>

int charToDecimal(const gchar * text){

  int puissance = 1;
  int result = 0;
  int i;

  for (i = strlen(text)-1; i >= 0; i --){
    result += puissance*(text[i]-'0');
    puissance *= 10;
  }

  return result;
}

int checkHost(struct MainWindow * windowHandler){

  int check = 0;
  int i;
  int y;
  const gchar * text;

  text = gtk_entry_get_text(GTK_ENTRY(windowHandler->hostInput));

  if (strlen(text) < 6) {
    return -1;
  }

  check = -1;

  for (i = 0; i < strlen(text); i++){
    if (text[i] == ' '){
      if (i < strlen(text)-4){
        check = 0;
      }
    }
  }

  return check;
}

int checkPort(struct MainWindow * windowHandler){

  int check = 0;
  int port;
  int i;
  const gchar * text;

  text = gtk_entry_get_text(GTK_ENTRY(windowHandler->portInput));

  for (i = 0; i < strlen(text);i++){
    if (text[i] < '0' || text[i] > '9'){
      check = -1;
    }
  }

  port = charToDecimal(text);

  if (port > 65535 || port < 1){
    check = -1;
  }
  return check;
}

int checkLenghts(struct MainWindow * windowHandler) {
  int check = 0;

  const gchar * minLengthStr = gtk_entry_get_text(GTK_ENTRY(windowHandler->bruteLengthStart));
  const gchar * maxLengthStr = gtk_entry_get_text(GTK_ENTRY(windowHandler->bruteLengthEnd));

  if (strlen(minLengthStr) <= 0 ){
    check = -1;
  }

  int minLength = charToDecimal(minLengthStr);
  int maxLength = charToDecimal(maxLengthStr);

  if ((minLength > maxLength) || (minLength < 1) || (maxLength > 20)){
    check = -1;
  }

  return check;
}
