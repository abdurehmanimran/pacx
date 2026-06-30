#ifndef PACKAGE_ATTR_H
#define PACKAGE_ATTR_H

typedef struct {
  char *arch;
  char *repo;
  char *fileName;
} packageAttr;

packageAttr *initPackageAttr();
void freePackageAttr(packageAttr *pkgAttr);
packageAttr *getPackageAttrs(char *url);

#endif
