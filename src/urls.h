#ifndef URLS_H
#define URLS_H

typedef char *packageURL;

packageURL getPackageURL(char *package, int ignoreDependencies);

#endif // URLS_H
