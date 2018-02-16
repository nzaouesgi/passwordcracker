#ifndef CHECK_H
#define CHECK_H

#include "../struct/gtkstruct.h"

int checkHost(struct MainWindow * windowHandler);
int checkPort(struct MainWindow * windowHandler);
int charToDecimal(const gchar * text);
int checkLenghts(struct MainWindow * windowHandler);

#endif
