#include "utils.h"
#include "colors.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

extern packageInfoList packageList;

void addAmount(const char *amount, double *total) {

  if (strstr(amount, "G"))
    *total += atof(amount) * 1024.0;
  else if (strstr(amount, "M"))
    *total += atof(amount);
  else if (strstr(amount, "K"))
    *total += atof(amount) / 1024.0;
  else
    *total += atof(amount) / (1024.0 * 1024.0);
}

String *chooseUnit(double amount) {
  char tempBuffer[24];
  char *format;

  if (amount >= 1000) {
    amount /= 1024;
    format = ((int)amount == amount) ? "%.0fGiB" : "%.1fGiB";
  } else if (amount * 1024 <= 1) {
    amount *= 1024 * 1024;
    format = ((int)amount == amount) ? "%0.fB" : "%.1fB";
  } else if (amount <= 1) {
    amount *= 1024;
    format = ((int)amount == amount) ? "%.0fKiB" : "%.1fKiB";
  } else
    format = ((int)amount == amount) ? "%.0fMiB" : "%.1fMiB";

  snprintf(tempBuffer, sizeof(tempBuffer), format, amount);
  return createString(tempBuffer);
}

void calcTotalSpeed(packageInfoList *packageList, String **totalSpeed,
                    String **totalDownloaded, double prevDownloaded) {
  double speedInMBs = 0;
  double downloadedInMBs = 0;

  for (int i = 0; i < packageList->n; i++) {
    addAmount(packageList->packages[i]->downloaded, &downloadedInMBs);

    if (packageList->packages[i]->progress == 100 ||
        packageList->packages[i]->notFinished == 0)
      continue;

    addAmount(packageList->packages[i]->speed, &speedInMBs);
  }

  downloadedInMBs += prevDownloaded;

  *totalSpeed = chooseUnit(speedInMBs);
  stringAppend(totalSpeed, "/s");
  *totalDownloaded = chooseUnit(downloadedInMBs);
}

void execute(char **args) {
  pid_t process;
  process = fork();

  if (process == 0)
    execvp(args[0], args);
  else
    waitpid(process, NULL, 0);
}

void movePackages() {
  char *mvArgs[4] = {"sh", "-c"};
  mvArgs[3] = NULL;

  // Creating the mv command
  String *moveArguments = createString("mv ");

  for (int i = 0; i < packageList.n; i++) {
    stringAppend(&moveArguments, DOWNLOAD_DIRECTORY);
    stringAppend(&moveArguments, packageList.packages[i]->packageName);
    stringAppend(&moveArguments, "* ");
  }
  stringAppend(&moveArguments, "/var/cache/pacman/pkg/ > /dev/null 2>&1");
  mvArgs[2] = moveArguments->str;

  execute(mvArgs);
  printf(GREEN "::" WHITE " Successfully moved " GREEN "%d" WHITE
               " packages!!\n",
         packageList.n);

  freeString(moveArguments);
}
