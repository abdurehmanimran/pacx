#include "packagelist.h"
#include "packageinfo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INIT_LIST_CAP 128

void initPackageList(packageInfoList *packageList) {
  packageList->n = 0;
  packageList->totalSize = 0;
  packageList->capacity = INIT_LIST_CAP;
  packageList->packages =
      (packageInfo **)malloc(sizeof(packageInfo *) * INIT_LIST_CAP);
  if (packageList->packages == NULL)
    puts("Error: Failed to allocate memory for packageList");
}

int insertPackage(packageInfoList *packageList, packageInfo *package,
                  double size) {
  if (packageList->capacity <= packageList->n) {
    packageList->capacity += 128;
    packageInfo **tempPtr = realloc(
        packageList->packages, (sizeof(packageInfo *) * packageList->capacity));
    if (tempPtr != NULL) {
      packageList->packages = tempPtr;
    } else {
      puts("Error: Failed to expand the packageList");
      return -1;
    }
  }
  packageList->n++;
  packageList->totalSize += size;
  packageList->packages[packageList->n - 1] = package;
  return packageList->n;
}

void popPackage(packageInfoList *packageList, packageInfo *package) {
  int index = 0;
  while (strcmp(packageList->packages[index]->packageName,
                package->packageName) != 0) {
    index++;
    if (index >= packageList->n)
      return; // Given package was not found in the list
  }

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
