#ifndef PACKAGE_INFO_H
#define PACKAGE_INFO_H

#include <stdatomic.h>

typedef struct {
  char *packageName;
  _Atomic(char *) downloaded;
  _Atomic(char *) speed;
  _Atomic(int) progress;
  _Atomic(int) notFinished;
  _Atomic(int) downloadStarted;
  int isRepo;
} packageInfo;

void initPackageInfo(packageInfo **package, char *name);
void freePackageInfo(packageInfo **package);
void changePackageInfo(packageInfo *package, int opt, char *newValue);

#endif // !PACKAGE_INFO_H
