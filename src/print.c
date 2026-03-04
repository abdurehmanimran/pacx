#include "print.h"
#include "colors.h"
#include "packagelist.h"
#include "progress.h"
#include <stdio.h>
#include <sys/ioctl.h>

void printHelp(int currentArg, char **argv) {
  printf(GREEN "Pacx\t" RED "A Pacman Wrapper\n");
  printf(GREEN "usage:\t" RED "pacx " WHITE "<operation> [...]\n");
  printf(GREEN "Operations:\n\t" WHITE);
  printf(RED "pacx " WHITE "{"
             "-S --sync" WHITE "}\n\t");
  printf(RED "pacx " WHITE "{-Su}\n\t");
  printf(RED "pacx " WHITE "{-h --help}\n");
}

int getTerminalWidth() {
  struct winsize ws;
  ioctl(0, TIOCGWINSZ, &ws);
  return ws.ws_col;
}

int calcColWidth(int percentage) {
  return (getTerminalWidth() * (double)percentage / 100);
}

void printCompleted(packageInfo *package) {
  printf("\x1B[K"); // Clear line
  printf(GREEN "%-*s"
               " %*s\n",
         calcColWidth(45), package->packageName, calcColWidth(50),
         RED "::" WHITE " Download Completed!!");
}

void printDownloadInfo(packageInfo *package) {
  if (package->packageName != NULL && package->speed != NULL &&
      package->downloaded != NULL && package->totalSize != NULL)
    printf(GREEN "%-*s" WHITE " %*s/s\t ", calcColWidth(45),
           package->packageName, calcColWidth(10), package->speed);
  printProgress(package->progress, calcColWidth(35));
  printf(RED "  %d%%\n" WHITE, package->progress);
}

void printDetails(packageInfoList *packageList) {
  printf(GREEN ":: No of Packages: " WHITE "%d\n", packageList->n);
  for (int i = 0; i < packageList->n; i++) {
    printf(GREEN "[%d]:\t" WHITE "%s\n", i + 1,
           packageList->packages[i]->packageName);
  }
}
