#include "packageinfo.h"
#include "packagelist.h"
#ifndef PACX_H
#define PACX_H

#define MOVE_TO_PREV_LINE printf("\x1b[1A")

typedef struct {
  char *arg;
  void (*operation)(int, char **);
} Argument;

void printHelp(int currentArg, char **argv);
void syncPackages(int currentArg, char **argv);
void updatePackages(int currentArg, char **argv);

#endif // !PACX_H
