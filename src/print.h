#ifndef PRINT_H
#define PRINT_H

#include "packagelist.h"

void printProgress(packageInfoList *packageList);
void printDetails(packageInfoList *packageList);
void printDownloadInfo(packageInfo *package);
void printHelp(int currentArg, char **argv);

#endif // !PRINT_H
