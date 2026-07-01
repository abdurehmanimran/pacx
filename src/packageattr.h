#ifndef PACKAGE_ATTR_H
#define PACKAGE_ATTR_H

#include "str.h"

typedef struct {
  char *repo;
  char *fileName;
  char *url;
} packageAttr;

packageAttr *initPackageAttr();
void freePackageAttr(packageAttr *pkgAttr);
String *getMirrorListPath(packageAttr *attr);

#endif
