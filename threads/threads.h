#ifndef THREADS_H
#define THREADS_H

#include "../struct/gtkstruct.h"
#include "../interface/interface.h"
#include <time.h>

void * startThreads (void * windowHandlerVoid);
int  countLogs (void * result, int argc, char ** argv, char ** colName);;
void getStats (struct AttackStack * stack, int done,int left,double timeSpent);

#endif
