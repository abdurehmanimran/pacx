#include "pacx.h"
#include "colors.h"
#include "downloader.h"
#include "packageinfo.h"
#include "packagelist.h"
#include "urls.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

Argument args[] = {{"-h", printHelp},        {"--help", printHelp},
                   {"-S", syncPackages},     {"-s", syncPackages},
                   {"--sync", syncPackages}, {"-Su", updatePackages}};

int totalArgs;
packageInfoList packageList;

int main(int argc, char **argv) {
  // Sudo Permission Check
  if (getegid() != 0) {
    puts(GREEN "Alert:" WHITE " You do not have " GREEN "sudo/root " WHITE
               "permissions!!");
    return 0;
  }

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

void printDownloadInfo(packageInfo *package) {
  if (strcmp(package->progress, "100%") == 0)
    printf("\x1b[2K" GREEN "%-20s Already Up to Date !!\n",
           package->packageName);

  else
    printf(GREEN "%-20sSpeed: " WHITE "%-10s\t " GREEN "Downloaded: " WHITE
                 "%-20s\t" GREEN "Total:" WHITE "%-20s\n",
           package->packageName, package->speed, package->downloaded,
           package->totalSize);
}

void printDetails(packageInfoList *packageList) {
  printf(GREEN ":: No of Packages: " WHITE "%d\n", packageList->n);
  for (int i = 0; i < packageList->n; i++) {
    printf(GREEN "[%d]: " WHITE "%s\n", i + 1,
           packageList->packages[i]->packageName);
  }
}

void *printProgress(void *arg) {
  int *isRunning = ((int *)arg);
  while (*isRunning) {

    for (int i = 0; i < packageList.n; i++) {
      if (packageList.packages[i]->downloaded != NULL &&
          packageList.packages[i]->speed != NULL)
        printDownloadInfo(packageList.packages[i]);
      else {
      }
    }
    fflush(stdout);
    fflush(stderr);
    if (*isRunning) {
      for (int i = 0; i < packageList.n; i++)
        MOVE_TO_PREV_LINE;
      sleep(1);
    }
  }
  return NULL;
}

void fetchPackages(packageInfoList *packageList) {
  int *isRunning; // Dynamically Allocated to be read by printing thread
  isRunning = (int *)malloc(sizeof(int));
  pthread_t *threads;
  createDownloadThreads(&threads, packageList);

  *isRunning = 1;
  pthread_t printingThread;
  pthread_create(&printingThread, NULL, printProgress, (void *)isRunning);

  waitForDownloadThreads(&threads, packageList);
  *isRunning = 0;
  pthread_join(printingThread, NULL);

  free(isRunning);
  free(threads);
}

void syncPackages(int currentArg, char **argv) {
  int *isRunning; // Dynamically Allocated to be read by printing thread
  isRunning = (int *)malloc(sizeof(int));
  // Setup the list of packages
  retrievePackages(currentArg, totalArgs, argv, &packageList);
  pthread_t *threads;
  createDownloadThreads(&threads, &packageList);

  *isRunning = 1;
  pthread_t printingThread;
  pthread_create(&printingThread, NULL, printProgress, (void *)isRunning);

  waitForDownloadThreads(&threads, &packageList);
  *isRunning = 0;
  pthread_join(printingThread, NULL);

  // Freeing Up Memory
  free(isRunning);
  for (int i = 0; i < packageList.n; i++)
    freePackageInfo(&packageList.packages[i]);
  free(packageList.packages);
  free(threads);
}

// Returns a malloced list of packages each on a separate line
char *getUpgradablePackakgeName() {
  FILE *process;
  char command[] = "pacman -Su --print-format %n";
  char packageNames[2056];
  char buffer[1024];

  if ((process = popen(command, "r")) == NULL) {
    puts(GREEN "Error:" WHITE " Failed to run pacman!!");
    exit(1);
  }

  if ((fgets(buffer, 1024, process) == NULL)) {
    // i.e Pacman executed without any output
    puts(GREEN "Alert:" WHITE " Nothing to do!!");
    exit(1);
  } else {
    strcpy(packageNames, buffer);
  }

  while (fgets(buffer, 1024, process) != NULL) {
    strcat(packageNames, buffer);
  }

  return strdup(packageNames);
}

void getUpgradablePackages(packageInfoList *packageList) {
  char *packageNames = getUpgradablePackakgeName();
  initPackageList(packageList);

  char *packageName = strtok(packageNames, "\n");
  if (packageName == NULL) {
    printf(GREEN "Error: " WHITE "Unable to separate packages from the list!!");
    exit(1);
  }

  // Separate pacakage names, Create packageInfo objects,
  // and Insert them into packageInfoList
  while (packageName != NULL) {
    packageInfo *package = (packageInfo *)malloc(sizeof(packageInfo));
    initPackageInfo(&package, packageName, getPackageURL(packageName, 1));
    insertPackage(packageList, package);
    packageName = strtok(NULL, "\n"); // Get the next packageName
  }

  // Print brief desciption about the packages that are to be updated
  printDetails(packageList);
  puts("");

  // Get the packages
  fetchPackages(packageList);
}

void updatePackages(int currentArg, char **argv) {
  int *isRunning;
  isRunning = (int *)malloc(sizeof(int));

  puts(GREEN "::" WHITE " Starting " GREEN "full system " WHITE "update!!");
  getUpgradablePackages(&packageList);
}
