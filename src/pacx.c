//                      Pacx - A Pacman Wrapper
// A simple hobby project by Abdur Rehman Imran <arehmanimran4@gmail.com>

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

int isSudo() {
  if (getegid() != 0) {
    puts(GREEN "Alert:" WHITE " You do not have " GREEN "sudo/root " WHITE
               "permissions!!");
    return 0;
  }
  return 1;
}

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
void printProgress() {
  while (1) {

    for (int i = 0; i < packageList.n; i++) {
      if (packageList.packages[i]->downloaded != NULL &&
          packageList.packages[i]->speed != NULL)
        printDownloadInfo(packageList.packages[i]);
    }
    fflush(stdout);
    fflush(stderr);
    int downloading = 0;
    for (int i = 0; i < packageList.n; i++) {
      downloading += packageList.packages[i]->notFinished;
    }
    if (downloading <= 0)
      break;

    // Move cursor n lines up
    for (int i = 0; i < (packageList.n); i++)
      MOVE_TO_PREV_LINE;
    sleep(1);
  }
  puts("");
}

void fetchPackages(packageInfoList *packageList) {
  pthread_t *threads;
  createDownloadThreads(&threads, packageList);

  printProgress();

  waitForDownloadThreads(&threads, packageList);

  free(threads);
}

void syncPackages(int currentArg, char **argv) {
  if (!isSudo())
    exit(1);
  // Setup the list of packages
  retrievePackages(currentArg, totalArgs, argv, &packageList);

  // Print the details
  printDetails(&packageList);
  printf("________________________________\n");

  // Getting the packages
  fetchPackages(&packageList);

  // Freeing the packageList
  freePackageList(&packageList);
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
    packageInfo *package;
    initPackageInfo(&package, packageName, getPackageURL(packageName, 1));
    insertPackage(packageList, package);
    packageName = strtok(NULL, "\n"); // Get the next packageName
  }
}

void updatePackages(int currentArg, char **argv) {
  if (!isSudo())
    exit(1);

  puts(GREEN "::" WHITE " Starting " GREEN "full system " WHITE "update!!");
  getUpgradablePackages(&packageList);

  // Print brief desciption about the packages that are to be updated
  printDetails(&packageList);
  puts("");

  // Get the packages
  fetchPackages(&packageList);

  // Free the malloced packages part of packaageList
  freePackageList(&packageList);
}
