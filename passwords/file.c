#include "../struct/gtkstruct.h"
#include "passwords.h"
#include <stdio.h>
#include <stdlib.h>

char ** getPasswordsFromFile(struct MainWindow * windowHandler, FILE * dictionnaryFile){

  char ** passwordsList;
  char c;
  off_t size;
  off_t i;
  int y;
  long o;
  windowHandler->crCount = 1;
  fseeko(dictionnaryFile,0,SEEK_END);
  size = ftello(dictionnaryFile);
  fseeko(dictionnaryFile,0,SEEK_SET);

  for (i = 0; i < size; i ++){
    if (fgetc(dictionnaryFile) == '\n'){
      windowHandler->crCount ++;
    }
  }

  passwordsList = NULL;
  passwordsList = (char **)malloc(sizeof(char*)*(windowHandler->crCount));
  if (passwordsList == NULL){
    return NULL;
  }

  for (i = 0; i < windowHandler->crCount; i ++){
    passwordsList[i] = NULL;
    passwordsList[i] = (char*)malloc(sizeof(char)*255);
    if (passwordsList[i] == NULL){
      return NULL;
    }
  }

  for (i = 0; i < windowHandler->crCount; i++){
    for (y = 0; y< 255; y++){
      passwordsList[i][y] = 0;
    }
  }

  i = 0;
  y = 0;
  int stopFlag = 0;

  fseeko(dictionnaryFile,0,SEEK_SET);
  c = fgetc(dictionnaryFile);

  while (stopFlag == 0){

    if (c == '\n'){
      passwordsList[i][y] = 0;
      i++;
      y = 0;
    } else if (c == EOF){
      passwordsList[i][y] = 0;
      stopFlag = 1;
    } else {
      passwordsList[i][y] = c;
      y ++;
    }

    if (y >= 99){
      for (o = 0; o < windowHandler->crCount; o ++){
        free(passwordsList[o]);
      }
      free(passwordsList);
      return NULL;
    }
    c = fgetc(dictionnaryFile);
  }

  return passwordsList;

}
