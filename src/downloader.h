#ifndef DOWNLOADER_H
#define DOWNLOADER_H
#include "packageinfo.h"
#include "packagelist.h"
#include <pthread.h>

// Interval after which the download info updates
#define UPDATE_INTERVAL "1"
// Directory in which the downloaded packages will be placed
#define DOWNLOAD_DIRECTORY "/usr/share/pacx/cache/"
#define DB_DIRECTORY "/var/lib/pacman/sync/"

void parseDetails(char *summary, packageInfo *package);
void downloadPackage(packageInfo *packageInformation);
void *startDownload(void *arg);

#endif // !DOWNLOADER_H
