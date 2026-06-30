#include "packageattr.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

packageAttr *initPackageAttr() {
  packageAttr *pkgAttr = NULL;
  pkgAttr = malloc(sizeof(packageAttr));

  if (!pkgAttr) {
    puts("Error: failed to allocate memory for packageAttr !!!");
  } else {
    memset(pkgAttr, 0, sizeof(packageAttr));
  }

  return pkgAttr;
}

void freePackageAttr(packageAttr *pkgAttr) {
  if (pkgAttr->arch)
    free(pkgAttr->arch);
  if (pkgAttr->repo)
    free(pkgAttr->repo);
  if (pkgAttr->fileName)
    free(pkgAttr->fileName);

  if (pkgAttr)
    free(pkgAttr);
}

packageAttr *getPackageAttrs(char *url) {
  char *i;
  char *savePtr;
  packageAttr *pkgAttr = initPackageAttr();

  i = strtok_r(url, "/", &savePtr);

  while (i != NULL && strstr(i, "x86_64") == NULL) {
    i = strtok_r(NULL, "/", &savePtr);
  }

  if (!i)
    goto nullCleanUp;
  pkgAttr->arch = strdup(i);
  i = strtok_r(NULL, "/", &savePtr);
  if (!i)
    goto nullCleanUp;
  pkgAttr->repo = strdup(i);
  i = strtok_r(NULL, "/", &savePtr);
  if (!i)
    goto nullCleanUp;
  pkgAttr->fileName = strdup(i);

  return pkgAttr;

nullCleanUp:
  freePackageAttr(pkgAttr);
  return NULL;
}
