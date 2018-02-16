#ifndef PASSWORDS_H
#define PASSWORDS_H

#include "../struct/gtkstruct.h"

char ** getPasswordsFromFile(struct MainWindow * windowHandler, FILE * dictionnaryFile);
void getPasswordsCount(struct MainWindow * windowHandler, int minLength,int maxLength,
char optMethodBruteUppercase, char optMethodBruteNumbers, char optMethodBruteSpecialChars);

#endif
