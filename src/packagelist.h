#include "packageinfo.h"
#ifndef PACKAGE_LIST_H
#define PACKAGE_LIST_H

typedef struct {
  int n;
  int capacity;
  double totalSize;
  packageInfo **packages;
} packageInfoList;

void initPackageList(packageInfoList *packageList);
int insertPackage(packageInfoList *packageList, packageInfo *package,
                  double size);
void freePackageList(packageInfoList *packageList);
void retrievePackages(int argPosition, int totalArgs, char **argv,
                      packageInfoList *packageList);
void popPackage(packageInfoList *packageList, packageInfo *package);

#endif // !PACKAGE_LIST_H
