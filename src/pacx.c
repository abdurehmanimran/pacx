//                      Pacx - A Pacman Wrapper
// A simple hobby project by Abdur Rehman Imran <arehmanimran4@gmail.com>

#include "pacx.h"
#include "colors.h"
#include "downloader.h"
#include "packageinfo.h"
#include "packagelist.h"
#include "print.h"
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

void fetchPackages(packageInfoList *packageList) {
  pthread_t *threads;
  createDownloadThreads(&threads, packageList);

  printProgress(packageList);

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
