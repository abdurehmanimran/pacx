#include "packageinfo.h"
#include <stdlib.h>
#include <string.h>

void initPackageInfo(packageInfo **package, char *name, char *url) {
  *package = malloc(sizeof(packageInfo));
  (*package)->packageName = strdup(name);
  (*package)->downloaded = strdup("0B");
  (*package)->speed = strdup("0B");
  (*package)->totalSize = strdup("0B");
  (*package)->url = url;
  (*package)->progress = 0;
  (*package)->notFinished = 1;
}

void freePackageInfo(packageInfo **package) {
  free((*package)->packageName);
  free((*package)->downloaded);
  free((*package)->speed);
  free((*package)->totalSize);
  free((*package)->url);
  free(*package);
}

void changePackageInfo(packageInfo *package, int opt, char *newValue) {
  char *temp;
  switch (opt) {
  case 1:
    temp = package->downloaded;
    package->downloaded = NULL;
    free(temp);
    package->downloaded = newValue;
    break;
  case 2:
    temp = package->totalSize;
    package->totalSize = NULL;
    free(temp);
    package->totalSize = newValue;
    break;
  case 3:
    temp = package->speed;
    package->speed = NULL;
    free(temp);
    package->speed = newValue;
    break;
  }
}
