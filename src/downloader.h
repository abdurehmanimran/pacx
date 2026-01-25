#ifndef DOWNLOADER_H
#define DOWNLOADER_H
#include "packageinfo.h"
#include "packagelist.h"
#include <pthread.h>

// Interval after which the download info updates
#define UPDATE_INTERVAL "1"
// Directory in which the downloaded packages will be placed
#define DOWNLOAD_DIRECTORY "/var/cache/pacman/pkg/"

void getDetails(char *summary, packageInfo **package);
void downloadPackage(packageInfo *packageInformation);
void *startDownload(void *arg);
void createDownloadThreads(pthread_t **threads, packageInfoList *packageList);
void waitForDownloadThreads(pthread_t **threads, packageInfoList *packageList);

#endif // !DOWNLOADER_H
