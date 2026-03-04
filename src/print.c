#include "print.h"
#include "colors.h"
#include "packagelist.h"
#include <stdio.h>

void printHelp(int currentArg, char **argv) {
  printf(GREEN "Pacx\t" RED "A Pacman Wrapper\n");
  printf(GREEN "usage:\t" RED "pacx " WHITE "<operation> [...]\n");
  printf(GREEN "Operations:\n\t" WHITE);
  printf(RED "pacx " WHITE "{"
             "-S --sync" WHITE "}\n\t");
  printf(RED "pacx " WHITE "{-Su}\n\t");
  printf(RED "pacx " WHITE "{-h --help}\n");
}

void printCompleted(packageInfo *package) {
  printf("\x1B[K"); // Clear line
  printf(GREEN "%-40s" RED "::" WHITE " Download Completed!!\n",
         package->packageName);
}

void printDownloadInfo(packageInfo *package) {
  if (package->packageName != NULL && package->speed != NULL &&
      package->downloaded != NULL && package->totalSize != NULL)
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
