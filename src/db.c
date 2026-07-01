#include "db.h"
#include "packageattr.h"
#include "str.h"
#include "urls.h"

#include <stdio.h>
#include <string.h>

void createDBPackageList(packageInfoList *dbList) {
  FILE *pacmanConf = fopen(PACMAN_CONFIG_PATH, "r");
  char lineBuff[128];
  memset(lineBuff, 0, sizeof(lineBuff));

  while (fgets(lineBuff, sizeof(lineBuff), pacmanConf)) {
    int end = 0;

    while (lineBuff[0] != '[' || strstr(lineBuff, "options")) {
      memset(lineBuff, 0, sizeof(lineBuff));
      fgets(lineBuff, sizeof(lineBuff), pacmanConf);

      if (feof(pacmanConf)) {
        end = 1;
        break;
      }
    }

    if (end)
      break;

    unsigned int endIndex = strcspn(lineBuff, "]");
    if (endIndex >= sizeof(lineBuff))
      endIndex = sizeof(lineBuff) - 1;
    lineBuff[endIndex] = 0;

    String *dbName = createString(lineBuff + 1);
    stringAppend(&dbName, ".db");

    packageInfo *db;
    initPackageInfo(&db, dbName->str);
    insertPackage(dbList, db);

    if (dbName)
      freeString(dbName);
  }

  fclose(pacmanConf);
}

String *getRepoMirrors(char *name) {
  String *mirrors = NULL;
  String *listPath = NULL;
  packageAttr *pkgAttr = initPackageAttr();
  char repoName[24];
  memset(repoName, 0, sizeof(repoName));
  strncpy(repoName, name, sizeof(repoName));

  repoName[strcspn(repoName, ".")] = 0;

  pkgAttr->fileName = strdup(name);
  pkgAttr->repo = strdup(repoName);
  pkgAttr->url = strdup("repo");

  listPath = getMirrorListPath(pkgAttr);
  if (!listPath)
    goto cleanup;

  mirrors = getRawMirrors(listPath->str, pkgAttr);

cleanup:
  if (pkgAttr != NULL)
    freePackageAttr(pkgAttr);
  if (listPath != NULL)
    freeString(listPath);

  return mirrors;
}
