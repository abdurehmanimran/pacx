#ifndef UTILS_H
#define UTILS_H

#include "downloader.h"
#include "packagelist.h"
#include "str.h"

// Unit Helper Funcs
void addAmount(const char *amount, double *total);
String *chooseUnit(double amount);
void calcTotalSpeed(packageInfoList *packageList, String **totalSpeed,
                    String **totalDownloaded, double prevDownloaded);

void execute(char **args);
void movePackages();

#endif
