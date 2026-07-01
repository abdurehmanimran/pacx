#ifndef PACKAGE_INFO_H
#define PACKAGE_INFO_H

typedef struct {
  char *packageName;
  char *downloaded;
  char *totalSize;
  char *speed;
  //  char *url; // getUrl func moved to downloader
  int progress;
  int notFinished;
  int downloadStarted;
  int isRepo;
} packageInfo;

void initPackageInfo(packageInfo **package, char *name);
void freePackageInfo(packageInfo **package);
void changePackageInfo(packageInfo *package, int opt, char *newValue);

#endif // !PACKAGE_INFO_H
