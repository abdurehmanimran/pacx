#ifndef PACX_H
#define PACX_H

#include "packageinfo.h"
#include "packagelist.h"
#include "str.h"

#define VERSION "0.1.4"
#define PARALLEL_DOWNLOADS 20

typedef struct {
  char *arg;
  void (*operation)();
} Argument;

int isSudo();
void syncPackages();
void updatePackages();
void syncDBs();

// Util Funcs
String *chooseUnit(double amount);

#endif // !PACX_H
