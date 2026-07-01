#ifndef PACKAGE_ATTR_H
#define PACKAGE_ATTR_H

#include "str.h"

#define PACMAN_CONFIG_PATH "/etc/pacman.conf"
#define ARCHITECTURE "x86_64"

typedef struct {
  char *repo;
  char *fileName;
  char *url;
} packageAttr;

packageAttr *initPackageAttr();
void freePackageAttr(packageAttr *pkgAttr);
String *getMirrorListPath(packageAttr *attr);

#endif
