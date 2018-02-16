#include <pthread.h>
#include "attack.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "../check/check.h"
#include "../struct/gtkstruct.h"
#include "../interface/interface.h"
#include "../check/check.h"
#include "../threads/threads.h"

void startAttack(struct MainWindow * windowHandler){

  //lancement de l'attaque.

  int check = 0;

  writeConsoleWithTag(windowHandler,"[ATTACK] ","strong_console");
  writeConsole(windowHandler,"Attack launched !\n");

  // v�rifications de champs.

  if (gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(windowHandler->protoList)) == NULL){
    writeConsoleWithTag(windowHandler, "[","strong_console");
    writeConsoleWithTag(windowHandler, "ERROR","error_console");
    writeConsoleWithTag(windowHandler, "] ","strong_console");

    writeConsole(windowHandler, "You need to select a protocol !\n");
    check = -1;
  }

  if (checkHost(windowHandler) == -1){
    writeConsoleWithTag(windowHandler, "[","strong_console");
    writeConsoleWithTag(windowHandler, "ERROR","error_console");
    writeConsoleWithTag(windowHandler, "] ","strong_console");

    writeConsole(windowHandler, "Host name is invalid ! Use \"user 192.168.1.11\" or \"user domain.com\" formats.\n");
    check = -1;
  }

  if (checkPort(windowHandler) == -1){
    writeConsoleWithTag(windowHandler, "[","strong_console");
    writeConsoleWithTag(windowHandler, "ERROR","error_console");
    writeConsoleWithTag(windowHandler, "] ","strong_console");

    writeConsole(windowHandler, "Port is invalid ! Port number must be between 1 and 65535.\n");
    check = -1;
  }

  if (windowHandler->optMethodDic == 1){
    if (strcmp(gtk_label_get_text(GTK_LABEL(windowHandler->fileInputLabel)),"No file selected.") == 0){
      writeConsoleWithTag(windowHandler, "[","strong_console");
      writeConsoleWithTag(windowHandler, "ERROR","error_console");
      writeConsoleWithTag(windowHandler, "] ","strong_console");

      writeConsole(windowHandler, "File error, please, select a valid file.\n");
      check = -1;
    }
  }

  if (windowHandler->optMethodBrute == 1){
    if (checkLenghts(windowHandler) == -1){
      writeConsoleWithTag(windowHandler, "[","strong_console");
      writeConsoleWithTag(windowHandler, "ERROR","error_console");
      writeConsoleWithTag(windowHandler, "] ","strong_console");

      writeConsole(windowHandler, "Password length is invalid ! Maximum length is 20 chars.\n");
      check = -1;
    }
  }

  // on stop en cas d'erreurs de syntaxe

  if (check == -1){
    g_signal_emit_by_name(G_OBJECT(windowHandler->stopButton),"clicked");
  }

  else {

    // lancement du processus de cracking

    const gchar * minLengthInput = gtk_entry_get_text(GTK_ENTRY(windowHandler->bruteLengthStart));
    const gchar * maxLengthInput = gtk_entry_get_text(GTK_ENTRY(windowHandler->bruteLengthEnd));
    windowHandler->minLength = charToDecimal(minLengthInput);
    windowHandler->maxLength = charToDecimal(maxLengthInput);
    windowHandler->threads = (gint) gtk_range_get_value(GTK_RANGE(windowHandler->threadsScale));

    // cr�ation du thread principal de cracking.

    pthread_create(&(windowHandler-> mainThread),NULL,startThreads,(void *)windowHandler);

  }
}

void stopAttack(struct MainWindow * windowHandler){

  writeConsoleWithTag(windowHandler,"[ATTACK] ","strong_console");
  writeConsole(windowHandler,"Attack stopped !\n");
}
