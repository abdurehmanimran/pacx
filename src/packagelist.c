#include "packagelist.h"
#include "packageinfo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void initPackageList(packageInfoList *packageList) {
  packageList->n = 0;
  packageList->capacity = 1;
  packageList->packages = (packageInfo **)malloc(sizeof(
      packageInfo *)); // Allocating memory for at least one ptr to packageInfo
  if (packageList->packages == NULL)
    puts("Error: Failed to allocate memory for packageList");
}

int insertPackage(packageInfoList *packageList, packageInfo *package) {
  packageList->n++;

  if (packageList->capacity < packageList->n) {
    packageList->capacity += 4;
    packageInfo **tempPtr = realloc(
        packageList->packages, (sizeof(packageInfo *) * packageList->capacity));
    if (tempPtr != NULL) {
      packageList->packages = tempPtr;
    } else {
      puts("Error: Failed to expand the packageList");
    }
  }

  packageList->packages[packageList->n - 1] = package;
  return packageList->n - 1;
}

void popPackage(packageInfoList *packageList, packageInfo *package) {
  int index = 0;
  while (strcmp(packageList->packages[index]->packageName,
                package->packageName) != 0)
    index++;

  for (int i = index; i < packageList->n - 1; i++) {
    packageList->packages[i] = packageList->packages[i + 1];
  }
  packageList->n--;
}

void freePackageList(packageInfoList *packageList) {
  for (int i = 0; i < packageList->n; i++)
    freePackageInfo(&packageList->packages[i]);
  free(packageList->packages);
}
