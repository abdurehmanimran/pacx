#include "packageattr.h"
#include "colors.h"
#include "str.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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
  if (pkgAttr->repo)
    free(pkgAttr->repo);
  if (pkgAttr->fileName)
    free(pkgAttr->fileName);
  if (pkgAttr->url)
    free(pkgAttr->url);

  if (pkgAttr)
    free(pkgAttr);
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

  // Go to [repoName]
  while (fgets(lineBuffer, sizeof(lineBuffer) - 1, pacmanConf) != NULL) {
    if (strstr(lineBuffer, repoName->str) == lineBuffer) {
      // fgets(lineBuffer, sizeof(lineBuffer) - 1, pacmanConf);
      break;
    }
    memset(lineBuffer, 0, sizeof(lineBuffer));
  }

  // Go to the line of Include = .....
  while (fgets(lineBuffer, sizeof(lineBuffer) - 1, pacmanConf) != NULL) {
    if (strstr(lineBuffer, "[") == lineBuffer || feof(pacmanConf)) {
      printf(RED "Error: " WHITE "failed to find the mirrorlist file !!\n");
      freeString(repoName);
      exit(1);
    }
    if (strstr(lineBuffer, "Include") == lineBuffer)
      break;

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
