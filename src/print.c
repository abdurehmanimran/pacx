#include "print.h"
#include "colors.h"
#include "packagelist.h"
#include "progress.h"
#include <stdio.h>
#include <string.h>
#include <sys/ioctl.h>

void printHelp() {
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
  int width = getTerminalWidth();
  if (width <= 100 && percentage >= 50) {
    percentage -= 5;
  } else if (width <= 120 && percentage >= 50) {
    percentage -= 1;
  }
  return (width * percentage / 100);
}

void printCompleted(packageInfo *package) {
  printf("\x1B[K"); // Clear line
  if (getTerminalWidth() >= 110)
    printf(GREEN "%-*s"
                 "%s\n",
           calcColWidth(60), package->packageName,
           RED "::" WHITE " Download Completed!!");
  else
    printf(GREEN "%-*s"
                 "%s\n",
           calcColWidth(50), package->packageName,
           RED "::" WHITE " Download Completed!!");
}

void printDownloadInfo(packageInfo *package) {
  // printf("\x1B[K"); // Clear line
  if (package->packageName != NULL && package->speed != NULL &&
      package->downloaded != NULL && package->totalSize != NULL) {

    if (getTerminalWidth() >= 110) { // For Normal Terminal Widths
      printf(GREEN "%-*.*s" WHITE "%*.*s%*.*s/s ", calcColWidth(43),
             calcColWidth(43) - 2, package->packageName, calcColWidth(8),
             calcColWidth(8), package->downloaded, calcColWidth(10) - 3,
             calcColWidth(10) - 4, package->speed);
      printf(GREEN);
      printProgress(package->progress, calcColWidth(39) - 4);
      printf(WHITE "%3d%%\n", package->progress);

    } else { // For Smaller Terminal Widths
      printf(GREEN "%-*.*s" WHITE "%*.*s%*.*s/s ", calcColWidth(30),
             calcColWidth(30) - 2, package->packageName, calcColWidth(14),
             calcColWidth(14), package->downloaded, calcColWidth(17) - 3,
             calcColWidth(17) - 4, package->speed);
      printf(GREEN);
      printProgress(package->progress, calcColWidth(39) - 4);
      printf(WHITE "%3d%%\n", package->progress);
    }
  } else {
    puts("");
  }
}

void printTotalStats(char *totalDownloaded, char *totalSpeed) {
  printf(GREEN);
  printProgress(100, 2);
  printf(RED " %s " WHITE, "Stats");
  printf(GREEN);
  if (getTerminalWidth() >= 110) {
    printProgress(100, calcColWidth(43) - strlen("Stats") - 2 - 2);
    printProgress(100, calcColWidth(8) - strlen(totalDownloaded) - 2);
    printf(RED " %s " GREEN, totalDownloaded);
    int sep = calcColWidth(9) - strlen(totalSpeed) - 1;
    printProgress(100, sep);
    if (sep < 1)
      printf(RED "%s " WHITE, totalSpeed);
    else
      printf(RED " %s " WHITE, totalSpeed);
    printf(GREEN);
    printProgress(100, calcColWidth(40) - 1);
  } else {
    printProgress(100, calcColWidth(30) - strlen("Stats") - 2 - 2);
    printProgress(100, calcColWidth(14) - strlen(totalDownloaded) - 2);
    printf(RED " %s " GREEN, totalDownloaded);
    int sep = calcColWidth(17) - strlen(totalSpeed) - 1;
    printProgress(100, sep);
    if (sep < 1)
      printf(RED "%s " WHITE, totalSpeed);
    else
      printf(RED " %s " WHITE, totalSpeed);
    printf(GREEN);
    printProgress(100, calcColWidth(39) - 1);
  }

  printf("\n");
}

void printDetails(packageInfoList *packageList) {
  printf(GREEN ":: No of Packages: " WHITE "%d\n", packageList->n);
  for (int i = 0; i < packageList->n; i++) {
    printf(GREEN "[%d]:\t" WHITE "%s\n", i + 1,
           packageList->packages[i]->packageName);
  }
}
