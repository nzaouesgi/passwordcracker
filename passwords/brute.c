#include "../struct/gtkstruct.h"
#include "passwords.h"
#include <math.h>
#include <string.h>

void getPasswordsCount(struct MainWindow * windowHandler, int minLength,int maxLength, char optMethodBruteUppercase,
char optMethodBruteNumbers, char optMethodBruteSpecialChars){

  int i;
  int y = 'a';
  int currentIndex = 26;
  windowHandler->bruteCount = 0;


  for (i = 0; i < 500; i ++){
    windowHandler->authorizedChars[i] = 0;
  }

  for (i = 0; i < 26; i ++){
    windowHandler->authorizedChars[i] = y;
    y++;
  }

  y = 'A';

  if (optMethodBruteUppercase){
    for (; i < currentIndex+26; i ++){
      windowHandler->authorizedChars[i] = y;
      y++;
    }
  }

  currentIndex = i;

  y = '0';

  if (optMethodBruteNumbers){
    for (; i < currentIndex+10; i ++){
      windowHandler->authorizedChars[i]  = y;
      y++;
    }
  }

  currentIndex = i;
  y = '!';

  if (optMethodBruteSpecialChars){
    for (; i < currentIndex+14; i ++){
      windowHandler->authorizedChars[i] = y;
      y++;
    }
  }

  for (i = minLength; i <= maxLength; i++ ){
    windowHandler->bruteCount += (unsigned long long) pow((double)strlen(windowHandler->authorizedChars),(double)i);
  }
}
