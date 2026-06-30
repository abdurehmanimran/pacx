#ifndef PACKAGE_ATTR_H
#define PACKAGE_ATTR_H

#include "str.h"

typedef struct {
  char *arch;
  char *repo;
  char *fileName;
  char *url;
} packageAttr;

packageAttr *initPackageAttr();
void freePackageAttr(packageAttr *pkgAttr);
packageAttr *getPackageAttrs(char *url);
String *getMirrorListPath(packageAttr *attr);

#endif
