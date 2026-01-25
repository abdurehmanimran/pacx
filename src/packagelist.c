#include "packagelist.h"
#include "packageinfo.h"
#include "urls.h"
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
      char *urls = getPackageURL(argv[argPosition], 0);
      char *url = NULL;
      char *token = strtok(urls, "\n");

      while (token != NULL) {
        if (strstr(token, argv[argPosition]) != NULL) {
          url = strdup(token);
        } else {
          // Urls for dependencies provided by pacman
          char *dependencyName = NULL;
          char *subtokens;
          char *subtok_r;
          subtokens = strtok_r(token, "/", &subtok_r);
          while (subtokens != NULL) {
            if (strstr(subtokens, ".pkg.tar.zst") != NULL) {
              subtokens[strcspn(subtokens, "-")] = '\0';
              break;
            }
            subtokens = strtok_r(NULL, "/", &subtok_r);
          }
          dependencyName = subtokens;
          char *dependencyURL = getPackageURL(dependencyName, 1);
          packageInfo *dependency;
          initPackageInfo(&dependency, subtokens, dependencyURL);
          insertPackage(packageList, dependency);
        }
        token = strtok(NULL, "\n");
      }
      if (urls != NULL)
        free(urls);

      initPackageInfo(&package, argv[argPosition], url);

      // Adding the packageInfo to the package
      insertPackage(packageList, package);
    } else {
      break;
    }
    argPosition++;
  }
}
