#ifndef PRINT_H
#define PRINT_H

#include "packagelist.h"

#define MOVE_N_LINES_UP(n) printf("\x1B[%dF", n)

void printProgress(packageInfoList *packageList);
void printDetails(packageInfoList *packageList);
void printDownloadInfo(packageInfo *package);
void printHelp(int currentArg, char **argv);

#endif // !PRINT_H
