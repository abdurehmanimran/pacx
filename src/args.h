#ifndef ARGS_H
#define ARGS_H

#include "str.h"

typedef struct {
  char *arg;
  void (*operation)();
} Argument;

extern char **arguments;
extern unsigned int totalArgs;
extern unsigned int currentArg;

void getArgumentPackages(String **buffer);
void getPackagesToIgnore(String **buffer);
char *getPackageNames(int toUpdate, String **command);

#endif
