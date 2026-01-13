#ifndef URLS_H
#define URLS_H

typedef char *packageURL;
typedef packageURL *URLs;

packageURL getPackageURL(char *package);
URLs getURLs(int n, char **packages);

#endif // URLS_H
