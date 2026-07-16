#ifndef URLS_H
#define URLS_H

#include "packageattr.h"
#include "packagelist.h"
#include "str.h"

typedef struct {
  String *repoName;
  String *mirrorUrls;
} repoURLs;

typedef struct {
  unsigned int n;
  unsigned int capacity;
  repoURLs **repos;
} mirrorTable;

String *getRawMirrors(char *path, packageAttr *attrs);
void fillURLPlaceholders(String **urls, packageAttr *attrs);
String *getUrls(mirrorTable *table, char *packageName);
void createMirrorTable(packageInfoList *dbList, mirrorTable **table);

#endif // URLS_H
