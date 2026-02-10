#include "colors.h"
#include "packagelist.h"
#include "pacx.h"
#include <stdio.h>
#include <unistd.h>

void printHelp(int currentArg, char **argv) {
  printf(GREEN "Pacx\t" RED "A Pacman Wrapper\n");
  printf(GREEN "usage:\t" RED "pacx " WHITE "<operation> [...]\n");
  printf(GREEN "Operations:\n\t" WHITE);
  printf(RED "pacx " WHITE "{"
             "-S --sync" WHITE "}\n\t");
  printf(RED "pacx " WHITE "{-Su}\n\t");
  printf(RED "pacx " WHITE "{-h --help}\n");
}

void printDownloadInfo(packageInfo *package) {
  if (package->progress >= 100) {
    printf("                                                                   "
           "            "
           "                   "
           "   \n");
    MOVE_TO_PREV_LINE;
    printf(GREEN "%-40s" RED "::" WHITE " Download Completed!!\n",
           package->packageName);
    return;
  } else
    printf(GREEN "%-30sSpeed: " WHITE "%-10s\t " GREEN "Downloaded: " WHITE
                 "%-10s\t" GREEN "Total: " WHITE "%-10s" GREEN
                 "Progress: " WHITE "%-5d\n",
           package->packageName, package->speed, package->downloaded,
           package->totalSize, package->progress);
}

void printDetails(packageInfoList *packageList) {
  printf(GREEN ":: No of Packages: " WHITE "%d\n", packageList->n);
  for (int i = 0; i < packageList->n; i++) {
    printf(GREEN "[%d]:\t" WHITE "%s\n", i + 1,
           packageList->packages[i]->packageName);
  }
}

int called = 0;
void printProgress(packageInfoList *packageList) {
  while (1) {

    for (int i = 0; i < packageList->n; i++) {
      if (packageList->packages[i]->downloaded != NULL &&
          packageList->packages[i]->speed != NULL)
        printDownloadInfo(packageList->packages[i]);
    }
    fflush(stdout);
    fflush(stderr);
    int downloading = 0;
    for (int i = 0; i < packageList->n; i++) {
      downloading += packageList->packages[i]->notFinished;
    }
    if (downloading <= 0)
      break;

    // Move cursor n lines up
    for (int i = 0; i < (packageList->n); i++)
      MOVE_TO_PREV_LINE;
    sleep(1);
  }
  puts("");
}
