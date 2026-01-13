#ifndef DOWNLOADER_H
#define DOWNLOADER_H

// Interval after which the download info updates
#define UPDATE_INTERVAL "1"

typedef struct {
  char *packageName;
  char *downloaded;
  char *totalSize;
  char *speed;
  char *progress;
} packageInfo;

void initPackageInfo(packageInfo *package, char *name);
void freePackageInfo(packageInfo *package);
void changePackageInfo(packageInfo *package, int opt, char *newValue);
void getDetails(char *summary, packageInfo **package);
void downloadPackage(packageInfo *packageInformation);

#endif // !DOWNLOADER_H
