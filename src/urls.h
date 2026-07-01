#ifndef URLS_H
#define URLS_H

#include "packageattr.h"
#include "str.h"

String *getRawMirrors(char *path, packageAttr *attrs);
void fillURLPlaceholders(String **urls, packageAttr *attrs);
packageAttr *getPackageAttr(char *package);
String *getMirrors(char *package);

#endif // URLS_H
