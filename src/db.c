#include "db.h"
#include "packageattr.h"
#include "str.h"

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
    insertPackage(dbList, db, 0);

    if (dbName)
      freeString(dbName);
  }

  fclose(pacmanConf);
}
