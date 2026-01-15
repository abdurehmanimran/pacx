#include "packageinfo.h"
#include <stdlib.h>
#include <string.h>

void initPackageInfo(packageInfo **package, char *name) {
  *package = malloc(sizeof(packageInfo));
  (*package)->packageName = name;
  (*package)->downloaded = strdup("0B");
  (*package)->progress = strdup("0%");
  (*package)->speed = strdup("0B");
  (*package)->totalSize = strdup("0B");
}

void freePackageInfo(packageInfo **package) {
  free((*package)->downloaded);
  free((*package)->speed);
  free((*package)->totalSize);
  free((*package)->progress);
  free(*package);
}

void changePackageInfo(packageInfo *package, int opt, char *newValue) {
  switch (opt) {
  case 1:
    free(package->downloaded);
    package->downloaded = newValue;
    break;
  case 2:
    free(package->totalSize);
    package->totalSize = newValue;
    break;
  case 3:
    free(package->speed);
    package->speed = newValue;
    break;
  case 4:
    free(package->progress);
    package->progress = newValue;
    break;
  }
}
