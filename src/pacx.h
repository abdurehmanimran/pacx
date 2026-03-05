#include "packageinfo.h"
#include "packagelist.h"
#ifndef PACX_H
#define PACX_H

typedef struct {
  char *arg;
  void (*operation)();
} Argument;

int isSudo();
// void printHelp(int currentArg, char **argv);
void syncPackages();
void updatePackages();

#endif // !PACX_H
