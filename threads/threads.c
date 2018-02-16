#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <unistd.h>
#include <sqlite3.h>
#include <time.h>
#include "../requests/requests.h"
#include "../struct/gtkstruct.h"
#include "../interface/interface.h"
#include "../passwords/passwords.h"
#include "../check/check.h"
#include "threads.h"

void *  startThreads (void * windowHandlerVoid){

  struct MainWindow * windowHandler = (struct MainWindow *) windowHandlerVoid;

  FILE * dictionnaryFile;
  int fileCheck;
  long i;
  long y;
  char ** listOfPasswords = NULL;
  const gchar * protocol = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(windowHandler->protoList));
  sqlite3 * db; // pointeur représentant la connection à la bdd
  char  sql [1000]; // la requête SQL
  int rc;
  char * errMsg;
  int done;
  char method [100];
  time_t chronometer;
  time_t beginningTime;
  // on met le nombre de lignes du fichiers dans une variable temporaire
  // (car on va la décrementer lors du cracking et on veut garder la valeur originale).
  unsigned long long numberOfPasswords;
  // le mot de passe pour se connecter à la BDD. on est obligés de le copier dans
  // une string temporaire car la fonction sqlite_key va modifier le buffer d'entrée
  //pour la clé secrète.
  char tmpDbPwd [200];
  strcpy(tmpDbPwd,windowHandler->dbPassword);

  //signal que le thread principal est démarré.
  windowHandler->stopped = 0;

  rc = sqlite3_open_v2("sqlite/data.db",&db,SQLITE_OPEN_READWRITE,NULL);

  if (rc != SQLITE_OK){
    printf("FATAL ERROR: Can't access the database.\n");
    pthread_exit(NULL);
  }
  // on rentre la clé pour que la BDD soit exploitable.
  sqlite3_key(db, tmpDbPwd, strlen(tmpDbPwd));

     if (windowHandler->optMethodDic == 1){
      fileCheck = 0;
      if (windowHandler->filename == NULL){
        fileCheck = -1;
      } else {
        dictionnaryFile = fopen(windowHandler->filename,"rb");
        if (dictionnaryFile == NULL){
          fileCheck = -1;
        }
      }

      if (fileCheck == -1){
         g_idle_add(endAttack,(gpointer)windowHandler);
      }
    }

     if (windowHandler->optMethodDic == 1){
       // récupération du fichier de mots de passe.
       // sert aussi à calculer le nombre de mots de passe (dans crCount)
      listOfPasswords = getPasswordsFromFile(windowHandler, dictionnaryFile);
      if (listOfPasswords == NULL){
        g_idle_add(endAttack,(gpointer)windowHandler);
        pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
        pthread_exit(NULL);
      }
      // pour l'inscription en BDD
      strcpy(method,"Dictionnary file: ");
      strcat(method,windowHandler->filename);
    }

    if (windowHandler->optMethodBrute == 1){
      // récupération du fichier bruteforce.
        getPasswordsCount(windowHandler, windowHandler->minLength,windowHandler->maxLength,
        windowHandler->optMethodBruteUppercase,
        windowHandler->optMethodBruteNumbers,
        windowHandler->optMethodBruteSpecialChars
      );

        if (windowHandler->bruteCount <= 0){
          g_idle_add(endAttack,(gpointer)windowHandler);
          pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
          pthread_exit(NULL);
        }
        // pour l'inscription en BDD
        strcpy(method,"Bruteforce");
    }

    if (windowHandler->optMethodBrute){
      numberOfPasswords = windowHandler->bruteCount;
    }
    else {
      numberOfPasswords = windowHandler->crCount;
      // placement au début du fichier de mdp.
      fseek(dictionnaryFile,0,SEEK_SET);
    }
    windowHandler->neededThreads = 0;
    // la contenu mémoire utilisé par les threads de cracking est envoyé via ce pointeur
    // de structure dit "AttackStack". On y trouve notamment les informations de l'attaque,
    // aussi un pointeur vers la structure de la fenêtre principale (pour afficher des messages par ex).
    struct AttackStack * stack = malloc(sizeof(*stack));
    stack->dic = windowHandler->optMethodDic;
    stack->brute = windowHandler->optMethodBrute;
    stack->handler = windowHandler;
    stack->host = gtk_entry_get_text(GTK_ENTRY(windowHandler->hostInput));
    stack->port = charToDecimal(gtk_entry_get_text(GTK_ENTRY(windowHandler->portInput)));
    stack->bruteIter = 0;
    stack->bruteCount= windowHandler->bruteCount;
    if (windowHandler->optMethodDic){
      stack->file = fopen(windowHandler->filename,"rb");
    }
    done = 0;
    //  on doit séparer le username et
    // l'adresse IP/nom de domaine du champ 'host'.
    // la séparation se fait au niveau de du charactère d'espacement.
    for (i = 0; i < strlen(stack->host); i ++){
      if (stack->host[i] == ' '){
        strcpy(stack->destination,(stack->host)+i+1);
        strncpy(stack->username,(stack->host),i);
      }
    }
    // on compte combien il y a d'entrées dans la table des logs.
    // pour savoir quel sera l'ID de la prochaine entrée.
    // cela se fait via la fonction de callback countLogs.
    sprintf(sql,"SELECT count(*) FROM logs;");
    int * result = malloc(sizeof(int));
    rc = sqlite3_exec(db, sql, countLogs, (void *)result, &errMsg);
    stack->attackId = *result;
    // on insère une nouvelle entrée dans la table. aves les informations de la table.
    // puis on ferme la BDD.
    sprintf(sql,"INSERT INTO logs (attackId,host,port,method) VALUES ('%d','%s','%s','%s');",*result,
    stack->host,gtk_entry_get_text(GTK_ENTRY(windowHandler->portInput)),method);
    rc = sqlite3_exec(db, sql, NULL, 0, &errMsg);
    free(result);
    sqlite3_close(db);
    // cela représente le temps du début de l'attaque.
    beginningTime = time(NULL);
    // la boucle principale de cracking. on décrémente le nombre de mots de passes à envoyer.
    // jusqu'à ce qu'il n'y ait plus de mot de passe ou que lorsque le signal d'arrêt survient.
    while ( numberOfPasswords > 0 && windowHandler->stopped == 0){
      // si le nombre de mots de passes est supérieur (ou égal)au nombre de threads, alors
      // tous les threads vont travailler. si le nombre de mots de passes restants est inférieur au
      // nombre de threads, alors on n'a besoin que du nombre de threads correspondant aux mots de passes
      // restants (dernière itération en théorie)
      if (numberOfPasswords >= windowHandler->threads){
        windowHandler->neededThreads = (long)windowHandler->threads;
      } else {
        windowHandler->neededThreads = numberOfPasswords;
      }
      // pour le nombre de threads nécessaires (avec comme maximum le nombre de threads entré
      //par l'utilisateur).
      for (i = 0; i < windowHandler->neededThreads; i++){
          /*
          création d'un thread qui ira travailler avec la fonction correspondante au protocole.
          pour ssh, on envoie à la fontion sendRequestSSH, qui gère le protocole SSH via la librairie
          libssh. pour tous les autres protocoles, on envoie à la fonction sendRequestCurl,
          celle ci gère tous les autres protocoles implémentés.
          */
          if (strcmp(protocol,"SSH") == 0){
            pthread_create(windowHandler->threadsId+i,NULL,sendRequestSSH,(void *)stack);
            done++;
          } else {
            pthread_create(windowHandler->threadsId+i,NULL,sendRequestCurl,(void *)stack);
            done++;
          }
      }
      // on attend que les threads terminent.
      for (i = 0; i < windowHandler->neededThreads;i++){
        pthread_join(windowHandler->threadsId[i],NULL);
      }
      // on sauvegarde le moment où les threads ont terminé et on calcule les statistiques
      // avant d'en relancer de nouveaux.
      chronometer = time(NULL);
      // calcul des statistiques.
      if (windowHandler->stopped == 0){
        getStats(stack,done,numberOfPasswords,(double)(chronometer-beginningTime));
      }
      // aussi bien sûr, on décrémente le nombre de mots de passes restants du nombre de threads qui
      // viennent d'être lancés.
      numberOfPasswords -= windowHandler->neededThreads;
    }
    // après le cracking, on libère la liste de mots de passes en mémoire (qui avaient été
    // utilisées lors du comptage des lignes).
    if (windowHandler->optMethodDic == 1 && listOfPasswords != NULL){
      for (i = 0; i < windowHandler->crCount; i++ ){
        free(listOfPasswords[i]);
      }
      free(listOfPasswords);
      fclose(dictionnaryFile);
    }

    free(stack);

    pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
    // si le signal d'arrêt n'a pas déjà été lancé. on le lance car c'est la fin de l'attaque.
    if (windowHandler->stopped == 0){
      g_idle_add(endAttack,(gpointer)windowHandler);
    }
    pthread_exit(NULL);
}

void getStats (struct AttackStack * stack, int done,int left,double timeSpent){

  int days = 0;
  int hours = 0;
  int minutes = 0;
  int seconds = 0;
  double timeLeft = (double)((double)left / (double)done) * timeSpent;
  int leftDays = 0;
  int leftHours = 0;
  int leftMinutes = 0;
  int leftSeconds = 0;

  while (timeSpent >= 86400){
    days ++;
    timeSpent-= 86400;
  }
  while (timeLeft >= 86400){
    leftDays ++;
    timeLeft-= 86400;
  }
  while (timeSpent >= 3600){
    hours ++;
    timeSpent-= 3600;
  }
  while (timeLeft >= 3600){
    leftHours ++;
    timeLeft-= 3600;
  }
  while (timeSpent >= 60){
    minutes ++;
    timeSpent-= 60;
  }
  while (timeLeft >= 60){
    leftMinutes ++;
    timeLeft-= 60;
  }
  if (timeSpent > 0){
    seconds = timeSpent;
  }
  if (timeLeft > 0){
    leftSeconds = timeLeft;
  }

  struct CallbackMessage * msg = malloc(sizeof(*msg));
  msg->handle = stack->handler;
  strcpy(msg->message,"[ATTACK] ");
  strcpy(msg->tagname,"strong_console");
  g_idle_add_full(100,writeConsoleWithTagCallback,(gpointer)msg,NULL);

  struct CallbackMessage * msg2 = malloc(sizeof(*msg2));
  msg2->handle = stack->handler;
  sprintf(msg2->message,"%d requests sent. %d to be done in %d day(s), %d hour(s), %d minute(s), %d second(s).\n"
  ,done,left,leftDays,leftHours,leftMinutes,leftSeconds);
  g_idle_add_full(101,writeConsoleCallback,(gpointer)msg2,NULL);
}

int countLogs (void * result, int argc, char ** argv, char ** colName){
  int * result1 = (int *) result;
  *result1 = atoi(argv[0]);
  return 0;
}
