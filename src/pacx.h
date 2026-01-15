#include "packageinfo.h"
#include "packagelist.h"
#ifndef PACX_H
#define PACX_H

typedef struct {
  char *arg;
  void (*operation)(int, char **);
} Argument;

void printHelp(int currentArg, char **argv);
void syncPackages(int currentArg, char **argv);

#endif // !PACX_H
