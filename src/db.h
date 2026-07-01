#ifndef DB_H
#define DB_H

#include "packageattr.h"
#include "packagelist.h"

void createDBPackageList(packageInfoList *dbList, packageInfoList *sigList);
String *getRepoMirrors(char *name);

#endif // !DB_H
