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
#include <unistd.h>

Argument args[] = {
    {"-h", printHelp},    {"--help", printHelp},    {"-S", syncPackages},
    {"-s", syncPackages}, {"--sync", syncPackages}, {"-Su", updatePackages},
};

int totalArgs;
char **arguments;
int currentArg;

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
    printHelp();
    return 0;
  } else {
    // Iterating through the arguments
    // To be Fixed !!! At this moment it only checks for the first argument
    for (size_t i = 0; i < (sizeof(args) / sizeof(Argument)); i++) {
      if (strcmp(args[i].arg, argv[1]) == 0) {
        currentArg = 1;
        arguments = argv;
        args[i].operation();
        return 0;
      }
    }
  }
  // Argument Management End
  return 0;
}

#define PARALLEL_DOWNLOADS 20

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

void chooseUnit(double amount, char *buffer) {
  char tempBuffer[24];

  if (amount >= 1000) {
    amount /= 1024;
    snprintf(tempBuffer, sizeof(tempBuffer), "%.1fGiB", amount);
  } else if (amount * 1024 <= 1) {
    amount *= 1024 * 1024;
    snprintf(tempBuffer, sizeof(tempBuffer), "%.1fB", amount);
  } else if (amount <= 1) {
    amount *= 1024;
    snprintf(tempBuffer, sizeof(tempBuffer), "%.1fKiB", amount);
  } else
    snprintf(tempBuffer, sizeof(tempBuffer), "%.1fMiB", amount);

  strcpy(buffer, tempBuffer);
}

void calcTotalSpeed(packageInfoList *packageList, char **totalSpeed,
                    char **totalDownloaded, double prevDownloaded) {
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

  char partialSpeed[24], partialDownloaded[24];
  chooseUnit(speedInMBs, partialSpeed);
  chooseUnit(downloadedInMBs, partialDownloaded);

  *totalDownloaded =
      (char *)malloc(sizeof(char) * (strlen(partialDownloaded) + 1));
  strcpy(*totalDownloaded, partialDownloaded);

  strcat(partialSpeed, "/s");
  *totalSpeed = (char *)malloc(sizeof(char) * (strlen(partialSpeed) + 1));
  strcpy(*totalSpeed, partialSpeed);
}

void fetchPackages(packageInfoList *packageList) {
  pthread_t *threads;
  double completedDownloaded = 0;

  packageInfoList packagesDownloading;
  initPackageList(&packagesDownloading);

  int initThreads = packageList->n >= PARALLEL_DOWNLOADS ? PARALLEL_DOWNLOADS
                                                         : packageList->n;
  int index = initThreads;

  threads = (pthread_t *)malloc(sizeof(pthread_t) * (initThreads));

  for (int i = 0; i < initThreads; i++) {
    insertPackage(&packagesDownloading, packageList->packages[i]);
    pthread_create(&threads[i], NULL, startDownload,
                   packagesDownloading.packages[packagesDownloading.n - 1]);
  }

  HIDE_CURSOR;
  while (1) {
    for (int i = 0; i < packagesDownloading.n; i++) {
      if (packagesDownloading.packages[i]->progress == 100 ||
          packagesDownloading.packages[i]->notFinished == 0) {
        printCompleted(packagesDownloading.packages[i]);
        addAmount(packagesDownloading.packages[i]->downloaded,
                  &completedDownloaded);
        popPackage(&packagesDownloading, packagesDownloading.packages[i]);
        if (index < packageList->n) {
          insertPackage(&packagesDownloading, packageList->packages[index++]);
          pthread_create(
              &threads[i], NULL, startDownload,
              packagesDownloading.packages[packagesDownloading.n - 1]);
        } else {
        }
      }
    }

    if (packagesDownloading.n <= 0) {
      puts("");
      free(threads);
      return;
    }

    for (int i = 0; i < packagesDownloading.n; i++) {
      printDownloadInfo(packagesDownloading.packages[i]);
    }

    char *totalSpeed, *totalDownloaded;
    calcTotalSpeed(packageList, &totalSpeed, &totalDownloaded,
                   completedDownloaded);
    printTotalStats(totalDownloaded, totalSpeed);

    MOVE_N_LINES_UP(packagesDownloading.n + 1);

    usleep(5000);
  }
  SHOW_CURSOR;

  free(threads);
}

void getArgumentPackages(char *buffer) {
  currentArg++;

  strcpy(buffer, arguments[currentArg]);
  strcat(buffer, " ");
  currentArg++;

  while (currentArg < totalArgs) {
    strcat(buffer, arguments[currentArg++]);
    strcat(buffer, " ");
  }
}

// Returns a malloced list of packages each on a separate line
char *getPackageNames(int toUpdate) {
  char command[2056];
  if (toUpdate)
    strcpy(command, "pacman -Su --print-format %n");
  else {
    strcpy(command, "pacman -S ");
    char argumentPackages[1024];
    getArgumentPackages(argumentPackages);
    strcat(command, argumentPackages);
    strcat(command, " --print-format %n");
  }

  FILE *process;
  char packageNames[5120];
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

void createPackageList(packageInfoList *packageList, int toUpdate) {
  char *packageNames = getPackageNames(toUpdate);
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

void execute(char **args) {
  pid_t process;
  process = fork();

  if (process == 0) {
    execvp(args[0], args);
  }
}

void syncPackages() {
  if (!isSudo())
    exit(1);

  // Setup the list of packages
  createPackageList(&packageList, 0);
  printDetails(&packageList);
  puts("");
  fetchPackages(&packageList);
  char *mvArgs[] = {"sh", "-c",
                    "mv /usr/share/pacx/cache/* /var/cache/pacman/pkg/", NULL};
  execute(mvArgs);
  printf(GREEN " ::" WHITE " Successfully moved" GREEN "%d" WHITE " packages!!",
         packageList.n);
  // Freeing the packageList
  freePackageList(&packageList);

  char argumentPackages[1024];
  currentArg = 1;
  getArgumentPackages(argumentPackages);

  int packages = totalArgs - 2;

  char **pacmanArgs = (char **)malloc(sizeof(char *) * (packages + 3));
  pacmanArgs[0] = "pacman";
  pacmanArgs[1] = "-S";
  pacmanArgs[packages + 2] = NULL;

  char *package = strtok(argumentPackages, " ");
  if (package != NULL)
    pacmanArgs[2] = package;

  for (int i = 1; i < packages; i++) {
    package = strtok(NULL, " ");
    pacmanArgs[i + 2] = package;
  }

  execvp(pacmanArgs[0], pacmanArgs);
}

void updatePackages() {
  if (!isSudo())
    exit(1);
  puts(GREEN "::" WHITE " Starting " GREEN "full system " WHITE "update!!");
  createPackageList(&packageList, 1);
  printDetails(&packageList);
  puts("");
  // Get the packages
  fetchPackages(&packageList);
  // Move the downladed packages
  char *mvArgs[] = {"sh", "-c",
                    "mv /usr/share/pacx/cache/* /var/cache/pacman/pkg/", NULL};
  execute(mvArgs);
  printf(GREEN " ::" WHITE " Successfully moved" GREEN "%d" WHITE " packages!!",
         packageList.n);

  // Free the malloced packages part of packaageList
  freePackageList(&packageList);

  char *pacmanArgs[] = {"pacman", "-Su", NULL};
  execvp(pacmanArgs[0], pacmanArgs);
}
