#include "packagelist.h"
#include "packageinfo.h"
#include <stdio.h>
#include <stdlib.h>

void initPackageList(packageInfoList *packageList) {
  packageList->n = 0;
  packageList->capacity = 1;
  packageList->packages = (packageInfo **)malloc(sizeof(
      packageInfo *)); // Allocating memory for at least one ptr to packageInfo
  if (packageList->packages == NULL)
    puts("Error: Failed to allocate memory for packageList");
}

void insertPackage(packageInfoList *packageList, packageInfo *package) {
  packageList->n++;

  if (packageList->capacity < packageList->n) {
    packageList->capacity += 4;
    packageInfo **tempPtr = realloc(
        packageList->packages, (sizeof(packageInfo *) * packageList->capacity));
    if (tempPtr != NULL)
      packageList->packages = tempPtr;
    else {
      puts("Error: Failed to expand the packageList");
    }
  }

  packageList->packages[packageList->n - 1] = package;
}

void retrievePackages(int argPosition, int totalArgs, char **argv,
                      packageInfoList *packageList) {
  argPosition++; // Move the position from "-S" to next one

  initPackageList(packageList);

  while (argPosition < totalArgs) {
    if (argv[argPosition][0] != '-') {
      // Making packageInfo from the argument
      packageInfo *package;
      initPackageInfo(&package, argv[argPosition]);

      // Adding the packageInfo to the package
      insertPackage(packageList, package);
    } else {
      break;
    }
    argPosition++;
  }
}
