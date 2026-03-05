#ifndef PRINT_H
#define PRINT_H

#include "packagelist.h"

#define MOVE_N_LINES_UP(n) printf("\x1B[%dF", n)
#define HIDE_CURSOR printf("\x1B[?25l")
#define SHOW_CURSOR printf("\x1B[?25h")

int getTerminalWidth();
void printDetails(packageInfoList *packageList);
void printCompleted(packageInfo *package);
void printDownloadInfo(packageInfo *package);
void printHelp();

#endif // !PRINT_H
