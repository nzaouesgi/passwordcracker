#ifndef REQUEST_H
#define REQUEST_H

#include "../struct/gtkstruct.h"

// on définit ici le décalage fixé en microsecondes représentant le temps à attendre
// lors d'une erreur timeout pendant l'envoi d'une requête. en cas d'erreurs consécutives,
// ce décalage va etre incrémenté DELAY_INCREMENT, qui lui même se multiplie par deux à chaque
// itération d'erreur. MAX_TRIES représente le nombre maximum d'erreurs par threads, au delà de
// ce nombre on considère que la requête ne peut pas aboutir (il y donc sûrement une erreur liée à
// l'utilisateur, mauvaise configuration ou problème réseau).

#define DEFAULT_DELAY 200000
#define DELAY_INCREMENT 100000
#define MAX_TRIES 5

void *  sendRequestSSH(void * attackstack);
void *  sendRequestCurl(void * attackstack);
gboolean timeOutError(gpointer user_data);
gboolean timeOutIncrease(gpointer user_data);
void authSuccess (gchar * password,struct AttackStack * attackStackPtr);
size_t write_data(void *buffer,size_t size,size_t nmemb,void* userp);
const char * getNextPassword(struct AttackStack * stack);
void getNextChar (struct AttackStack * stack, int i);
#endif
