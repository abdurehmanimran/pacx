#include "print.h"
#include "colors.h"
#include "packagelist.h"
#include "pacx.h"
#include "progress.h"
#include "str.h"
#include "utils.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

void printHelp() {
  printf(GREEN "Pacx\t" RED "A Pacman Wrapper\n");
  printf(GREEN "ver:\t" RED VERSION "\n");
  printf(GREEN "usage:\t" RED "pacx " WHITE "<operation> [...] <options> \n");
  printf(GREEN "Operations:\n\t" WHITE);
  printf(RED "pacx " WHITE "{"
             "-S" WHITE "} _____ ____\n\t");
  printf(RED "pacx " WHITE "{-Su}\n\t");
  printf(RED "pacx " WHITE "{-Sy/yu}\n\t");
  printf(RED "pacx " WHITE "{-R/c/s/cs} _____ ____\n\t");
  printf(RED "pacx " WHITE "{-h --help}\n");
  printf(GREEN "Options:\n\t" WHITE);
  printf(RED "pacx " WHITE "{-S/u/yu} --ignore _____ ____\n");
}

int getTerminalWidth() {
  struct winsize ws;
  ioctl(0, TIOCGWINSZ, &ws);
  return ws.ws_col;
}

int calcColWidth(int percentage) {
  int width = getTerminalWidth();
  if (width <= 100 && percentage >= 50) {
    percentage -= 5;
  } else if (width <= 120 && percentage >= 50) {
    percentage -= 1;
  }
  return (width * percentage / 100);
}

String *getName(char *packageName) {
  String *name = NULL;
  // printf("\x1B[K"); // Clear line
  if (strstr(packageName, ".db")) {
    name = createString(":: ");
    char *dup = strdup(packageName);
    dup[strcspn(dup, ".")] = 0;
    stringAppend(&name, dup);
    free(dup);
  } else
    name = createString(packageName);

  return name;
}

void printCompleted(packageInfo *package) {
  String *name = getName(package->packageName);

  printf("\x1B[K"); // Clear line
  const int nameIndent = calcColWidth(getTerminalWidth() >= 110 ? 60 : 64);
  printf(GREEN "%-*s"
               "%s\n",
         nameIndent, name->str, RED "::" WHITE " Download Completed!!");

  if (name)
    freeString(name);
}

void printDownloadInfo(packageInfo *package) {
  String *name = getName(package->packageName);

  if (package->packageName == NULL || package->speed == NULL ||
      package->downloaded == NULL) {
    printf("\n");
    return;
  }

  int isBigScreen = getTerminalWidth() >= 110;
  const int nameIndent = calcColWidth(isBigScreen ? 43 : 30);
  const int downloadedIndent = calcColWidth(isBigScreen ? 8 : 14);
  const int speedIndent = calcColWidth(isBigScreen ? 10 : 17) - 3;
  const int progressIndent = calcColWidth(39) - (isBigScreen ? 3 : 4);

  printf(GREEN "%-*.*s" WHITE "%*.*s%*.*s/s ", nameIndent, nameIndent,
         name->str, downloadedIndent, downloadedIndent, package->downloaded,
         speedIndent, speedIndent - 1, package->speed);
  printf(GREEN);
  printProgress(package->progress, progressIndent);
  printf(WHITE "%3d%%\n", package->progress);

  if (name)
    freeString(name);
}

void printTotalStats(char *totalDownloaded, char *totalSpeed) {
  printf(GREEN);
  printProgress(100, 2);
  printf(RED " %s " WHITE, "Stats");
  printf(GREEN);

  int isBigScreen = getTerminalWidth() >= 110;
  const int nameIndent = calcColWidth(isBigScreen ? 43 : 30) - strlen("Stats") -
                         (isBigScreen ? 3 : 4); // 2 for the spaces around Stats
  const int downloadedIndent = calcColWidth(isBigScreen ? 8 : 14) -
                               strlen(totalDownloaded) - (isBigScreen ? 2 : 1);
  const int separationIndent = calcColWidth(isBigScreen ? 10 : 17) -
                               strlen(totalSpeed) - (isBigScreen ? 3 : 3);
  const int progressIndent =
      calcColWidth(isBigScreen ? 40 : 39) - (isBigScreen ? 1 : 0);

  printProgress(100, nameIndent);
  printProgress(100, downloadedIndent);
  printf(RED " %s " GREEN, totalDownloaded);
  printProgress(100, separationIndent);
  if (separationIndent < 1)
    printf(RED "%s " WHITE, totalSpeed);
  else
    printf(RED " %s " WHITE, totalSpeed);
  printf(GREEN);
  printProgress(100, progressIndent);

  printf("\n");
}

void printDetails(packageInfoList *packageList) {
  printf(GREEN ":: No of Packages: " WHITE "%d\n", packageList->n);
  for (int i = 0; i < packageList->n; i++) {
    printf(GREEN "[%d]:\t" WHITE "%s\n", i + 1,
           packageList->packages[i]->packageName);
  }
  String *totalSize = chooseUnit(packageList->totalSize);
  printf(GREEN ":: Total Download Size: " WHITE "%s\n", totalSize->str);
  freeString(totalSize);
}

int askYesOrNo() {
  char buff[12];
  printf(GREEN ":: Do you want to continue? " WHITE "[Y/n] : ");
  int stat = scanf("%10s", buff);

  if (stat != 1) {
    printf(RED "Error: " WHITE "invalid input given !!\n");
    exit(1);
  }
  return buff[0] == 'y' || buff[0] == 'Y';
}
