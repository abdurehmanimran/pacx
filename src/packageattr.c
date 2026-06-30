#include "packageattr.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PACMAN_CONFIG_PATH "/etc/pacman.conf"

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
  puts(i);
  pkgAttr->arch = strdup(i);
  i = strtok_r(NULL, "/", &savePtr);
  if (!i)
    goto nullCleanUp;
  puts(i);
  pkgAttr->repo = strdup(i);
  i = strtok_r(NULL, "/", &savePtr);
  if (!i)
    goto nullCleanUp;
  puts(i);
  pkgAttr->fileName = strdup(i);

  return pkgAttr;

nullCleanUp:
  freePackageAttr(pkgAttr);
  return NULL;
}

String *getMirrorListPath(packageAttr *attr) {
  if (!attr) {
    puts("Error: no attributes provided !!");
    return NULL;
  }

  String *path;
  String *repoName = createString("[");
  stringAppend(&repoName, attr->repo);
  stringAppend(&repoName, "]");

  FILE *pacmanConf = fopen(PACMAN_CONFIG_PATH, "r");

  char lineBuffer[128];
  memset(lineBuffer, 0, sizeof(lineBuffer));

  while (fgets(lineBuffer, sizeof(lineBuffer) - 1, pacmanConf) != NULL) {
    if (strstr(lineBuffer, repoName->str) == lineBuffer) {
      fgets(lineBuffer, sizeof(lineBuffer) - 1, pacmanConf);
      break;
    }
    memset(lineBuffer, 0, sizeof(lineBuffer));
  }
  unsigned int newLineIndex = strcspn(lineBuffer, "\n");
  if (newLineIndex >= sizeof(lineBuffer) - 1)
    newLineIndex = sizeof(lineBuffer) - 1;

  lineBuffer[newLineIndex] = '\0';

  path = createString(lineBuffer + 10);

  fclose(pacmanConf);
  freeString(repoName);

  return path;
}
