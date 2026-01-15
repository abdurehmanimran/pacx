#include "packageinfo.h"

#ifndef DOWNLOADER_H
#define DOWNLOADER_H

// Interval after which the download info updates
#define UPDATE_INTERVAL "1"

void getDetails(char *summary, packageInfo **package);
void downloadPackage(packageInfo *packageInformation);

#endif // !DOWNLOADER_H
