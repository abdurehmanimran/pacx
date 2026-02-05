#include "packageinfo.h"
#ifndef PACKAGE_LIST_H
#define PACKAGE_LIST_H

typedef struct {
  int n;
  int capacity;
  packageInfo **packages;
} packageInfoList;

void initPackageList(packageInfoList *packageList);
void insertPackage(packageInfoList *packageList, packageInfo *package);
void freePackageList(packageInfoList *packageList);
void retrievePackages(int argPosition, int totalArgs, char **argv,
                      packageInfoList *packageList);

#endif // !PACKAGE_LIST_H
