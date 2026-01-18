#include "pacx.h"
#include "colors.h"
#include "downloader.h"
#include "packageinfo.h"
#include "packagelist.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

Argument args[] = {{"-h", printHelp},
                   {"--help", printHelp},
                   {"-S", syncPackages},
                   {"-s", syncPackages},
                   {"--sync", syncPackages}};

int totalArgs;
packageInfoList packageList;

int main(int argc, char **argv) {
  totalArgs = argc;
  // Argument Management Start
  if (argc == 1) {
    printHelp(1, argv);
    return 0;
  } else {
    // Iterating through the arguments
    // To be Fixed !!! At this moment it only checks for the first argument
    for (int i = 1; i < (sizeof(args) / sizeof(Argument)); i++) {
      if (strcmp(args[i].arg, argv[1]) == 0) {
        args[i].operation(1, argv);
        return 0;
      }
    }
  }
  // Argument Management End
  return 0;
}

void printHelp(int currentArg, char **argv) {
  printf(GREEN "Pacx\t" WHITE "A pacman wrapper\n" GREEN "usage:\t" WHITE
               "pacx <operation> [...]\n" GREEN "operations:\n\t" WHITE
               "pacx {-h --help}\n");
}

void *startDownload(void *arg) { downloadPackage((packageInfo *)arg); }

void printProgress() {
  for (int i = 0; i < packageList.n; i++) {
    if (packageList.packages[i]->downloaded != NULL &&
        packageList.packages[i]->speed != NULL) {
      printf(GREEN "%s\t\t\tSpeed: " WHITE "%s\t " GREEN "Downloaded: " WHITE
                   "%s\n",
             packageList.packages[i]->packageName,
             packageList.packages[i]->speed,
             packageList.packages[i]->downloaded);
    } else {
    }
  }
  fflush(stdout);
  fflush(stderr);
}

void syncPackages(int currentArg, char **argv) {
  // Setup the list of packages
  retrievePackages(currentArg, totalArgs, argv, &packageList);
  pthread_t *threads;
  threads = (pthread_t *)malloc(sizeof(pthread_t) * packageList.n);

  for (int i = 0; i < packageList.n; i++) {
    printf(GREEN "Launching a thread for Package [%d]: " WHITE "%s\n", i + 1,
           packageList.packages[i]->packageName);

    // Starting the threads
    pthread_create(&threads[i], NULL, startDownload, packageList.packages[i]);
  }

  while (1) {
    printf("\r");
    printProgress();
    for (int i = 0; i < packageList.n; i++)
      printf("\x1b[1A");
    sleep(1);
  }

  for (int i = 0; i < packageList.n; i++) {
    pthread_join(threads[i], NULL);
  }
}
