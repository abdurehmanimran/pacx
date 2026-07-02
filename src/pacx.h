#include "packageinfo.h"
#include "packagelist.h"
#ifndef PACX_H
#define PACX_H

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

#endif // !PACX_H
