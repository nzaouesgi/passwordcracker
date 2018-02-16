#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "requests.h"
#include "../interface/interface.h"
#include "../struct/gtkstruct.h"

gboolean timeOutError(gpointer user_data){

  struct AttackStack * attackStackPtr = (struct AttackStack*) user_data;

  writeConsoleWithTag(attackStackPtr->handler,"[","strong_console");
  writeConsoleWithTag(attackStackPtr->handler,"ERROR","error_console");
  writeConsoleWithTag(attackStackPtr->handler,"] ","strong_console");
  writeConsole(attackStackPtr->handler,"Warning: ");
  writeConsole(attackStackPtr->handler,attackStackPtr->errorMessage);
  writeConsole(attackStackPtr->handler,"\n");
  return FALSE;
}

gboolean timeOutIncrease(gpointer user_data){

  struct AttackStack * attackStackPtr = (struct AttackStack*) user_data;

  writeConsoleWithTag(attackStackPtr->handler,"[","strong_console");
  writeConsoleWithTag(attackStackPtr->handler,"ERROR","error_console");
  writeConsoleWithTag(attackStackPtr->handler,"] ","strong_console");
  writeConsole(attackStackPtr->handler,"Request timed out. Adding delay (unstable).\n");
  return FALSE;
}

size_t write_data(void *buffer,size_t size,size_t nmemb,void* userp){
  return size * nmemb;
}

void authSuccess (gchar * password,struct AttackStack * attackStackPtr){


  sqlite3 * db; // pointeur représentant la connection à la bdd
  char  sql [500]; // la requête SQL
  int rc;
  char * errMsg;
  char tmpDbPwd [200];
  strcpy(tmpDbPwd,(attackStackPtr->handler)->dbPassword);
  rc = sqlite3_open_v2("sqlite/data.db",&db,SQLITE_OPEN_READWRITE,NULL);

  if (rc != SQLITE_OK){
    printf("FATAL ERROR: Can't access the database.\n");
  }

  sqlite3_key(db,tmpDbPwd , strlen(tmpDbPwd));

  sprintf(sql,"UPDATE logs SET pwdDump = '%s',success = 1 WHERE attackId = %d;",password,attackStackPtr->attackId);
  rc = sqlite3_exec(db, sql, NULL, 0, &errMsg);
  if (rc != SQLITE_OK){
    printf("Something wrong happened with updating the database: %s\n", errMsg);
  }

  attackStackPtr->handler->stopped = 1;

  struct CallbackMessage * success = malloc(sizeof(*success));
  success->handle = attackStackPtr->handler;
  strcpy(success->message,"[");
  strcpy(success->tagname,"strong_console");
  g_idle_add_full(110,writeConsoleWithTagCallback,(gpointer)success,NULL);

  struct CallbackMessage * success1 = malloc(sizeof(*success1));
  success1->handle = attackStackPtr->handler;
  strcpy(success1->message,"SUCCESS");
  strcpy(success1->tagname,"success_console");
  g_idle_add_full(111,writeConsoleWithTagCallback,(gpointer)success1,NULL);

  struct CallbackMessage * success2 = malloc(sizeof(*success2));
  success2->handle = attackStackPtr->handler;
  strcpy(success2->message,"] ");
  strcpy(success2->tagname,"strong_console");
  g_idle_add_full(112,writeConsoleWithTagCallback,(gpointer)success2,NULL);
  
  struct CallbackMessage * msg = malloc(sizeof(*msg));
  msg->handle = attackStackPtr->handler;
  sprintf(msg->message,"Key found for user '%s' ! Don't rely on luck next time... ;)\n***SECRET KEY*** => ",attackStackPtr->username);
  g_idle_add_full(113,writeConsoleCallback,(gpointer)msg,NULL);

  struct CallbackMessage * msg2 = malloc(sizeof(*msg2));
  msg2->handle = attackStackPtr->handler;
  strcpy(msg2->message,password);
  strcpy(msg2->tagname,"success_console");
  g_idle_add_full(114,writeConsoleWithTagCallback,(gpointer)msg2,NULL);

  struct CallbackMessage * msg3 = malloc(sizeof(*msg3));
  msg3->handle = attackStackPtr->handler;
  strcpy(msg3->message,"\n");
  g_idle_add_full(115,writeConsoleCallback,(gpointer)msg3,NULL);

  g_idle_add(endAttack,(gpointer)attackStackPtr->handler);
}

const char * getNextPassword(struct AttackStack * stack){

  int i;
  int y;
  int check;

  // creation de la première chaîne en fonction du minLength.

  if (stack->bruteIter == 0){
    strcpy(stack->brutePwd,"a");
    for (i = 0; i < stack->handler->minLength -1;i ++){
      strcat(stack->brutePwd,"a");
    }
    stack->bruteIter++;
    return stack->brutePwd;
  }

  // si tous les caractères ont pour valeur le dernier caractère de authorizeChars,
  // on rajoute un caractère à la chaîne, et on les re initialise les valeurs à 'a'.

  check = 1;

  for (i = 0; i < strlen(stack->brutePwd); i++){
    if (stack->brutePwd[i] !=
      stack->handler->authorizedChars[strlen(stack->handler->authorizedChars)-1]){
      check = 0;
    }
  }

  if (check == 1){
    for (i = 0; i < strlen(stack->brutePwd);i++){
      stack->brutePwd[i] = 'a';
    }
    strcat(stack->brutePwd,"a");
    stack->bruteIter++;
    return stack->brutePwd;
  }

  for (i = strlen(stack->brutePwd)-1; i >= 0; i--){
    if (stack->brutePwd[i] !=
      stack->handler->authorizedChars[strlen(stack->handler->authorizedChars)-1]){
        getNextChar(stack,i);
        for (y = i+1; y < strlen(stack->brutePwd); y ++){
          stack->brutePwd[y] = 'a';
        }
        break;
      }
  }
  stack->bruteIter++;
  return stack->brutePwd;
}

void getNextChar (AttackStack * stack, int i){
  int y;

  for (y = 0; y < strlen(stack->handler->authorizedChars); y ++){
    if (stack->brutePwd[i] == stack->handler->authorizedChars[y]){
      stack->brutePwd[i] = stack->handler->authorizedChars[y+1];
      break;
    }
  }
}
