//                      Pacx - A Pacman Wrapper
// A simple hobby project by Abdur Rehman Imran <arehmanimran4@gmail.com>

#include "pacx.h"
#include "colors.h"
#include "downloader.h"
#include "packageinfo.h"
#include "packagelist.h"
#include "print.h"
#include "str.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PARALLEL_DOWNLOADS 20

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

void chooseUnit(double amount, String **buffer) {
  char tempBuffer[24];
  char *format;

  if (amount >= 1000) {
    amount /= 1024;
    format = ((int)amount == amount) ? "%.0fGiB" : "%.1fGiB";
  } else if (amount * 1024 <= 1) {
    amount *= 1024 * 1024;
    format = ((int)amount == amount) ? "%0.fB" : "%.1fB";
  } else if (amount <= 1) {
    amount *= 1024;
    format = ((int)amount == amount) ? "%.0fKiB" : "%.1fKiB";
  } else
    format = ((int)amount == amount) ? "%.0fMiB" : "%.1fMiB";

  snprintf(tempBuffer, sizeof(tempBuffer), format, amount);
  *buffer = createString(tempBuffer);
}

void calcTotalSpeed(packageInfoList *packageList, String **totalSpeed,
                    String **totalDownloaded, double prevDownloaded) {
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

  chooseUnit(speedInMBs, totalSpeed);
  stringAppend(totalSpeed, "/s");
  chooseUnit(downloadedInMBs, totalDownloaded);
}

void fetchPackages(packageInfoList *packageList) {
  pthread_t *threads;
  double completedDownloaded = 0;

  packageInfoList *packagesDownloading = malloc(sizeof(packageInfoList));
  initPackageList(packagesDownloading);

  int initThreads = packageList->n >= PARALLEL_DOWNLOADS ? PARALLEL_DOWNLOADS
                                                         : packageList->n;
  int index = initThreads;

  threads = (pthread_t *)malloc(sizeof(pthread_t) * (initThreads));

  for (int i = 0; i < initThreads; i++) {
    insertPackage(packagesDownloading, packageList->packages[i]);
    pthread_create(&threads[i], NULL, startDownload,
                   packagesDownloading->packages[packagesDownloading->n - 1]);
  }

  HIDE_CURSOR;
  while (1) {
    for (int i = 0; i < packagesDownloading->n; i++) {
      if (packagesDownloading->packages[i]->progress == 100 ||
          packagesDownloading->packages[i]->notFinished == 0) {
        printCompleted(packagesDownloading->packages[i]);
        addAmount(packagesDownloading->packages[i]->downloaded,
                  &completedDownloaded);
        popPackage(packagesDownloading, packagesDownloading->packages[i]);
        if (index < packageList->n) {
          insertPackage(packagesDownloading, packageList->packages[index++]);
          pthread_create(
              &threads[i], NULL, startDownload,
              packagesDownloading->packages[packagesDownloading->n - 1]);
        } else {
        }
      }
    }

    if (packagesDownloading->n <= 0) {
      puts("");
      goto cleanup;
    }

    for (int i = 0; i < packagesDownloading->n; i++) {
      printDownloadInfo(packagesDownloading->packages[i]);
    }

    String *totalSpeed;
    String *totalDownloaded;
    calcTotalSpeed(packageList, &totalSpeed, &totalDownloaded,
                   completedDownloaded);
    printTotalStats(totalDownloaded->str, totalSpeed->str);

    freeString(totalSpeed);
    freeString(totalDownloaded);

    MOVE_N_LINES_UP(packagesDownloading->n + 1);

    usleep(5000);
  }
  SHOW_CURSOR;

cleanup:
  freePackageList(packagesDownloading);
  free(packagesDownloading);
  free(threads);
}

void getArgumentPackages(String **buffer) {
  allocString(buffer, 1024);
  currentArg++;

  stringAppend(buffer, arguments[currentArg]);
  stringAppend(buffer, " ");
  currentArg++;

  while (currentArg < totalArgs) {
    stringAppend(buffer, arguments[currentArg++]);
    stringAppend(buffer, " ");
  }
}

// Returns a malloced list of packages each on a separate line
char *getPackageNames(int toUpdate) {
  String *command;
  String *argumentPackages = NULL;
  String *packageNames;

  if (toUpdate)
    command = createString("pacman -Su --print-format %n");
  else {
    command = createString("pacman -S ");

    getArgumentPackages(&argumentPackages);
    stringCat(&command, argumentPackages);
    stringAppend(&command, " --print-format %n");
  }

  FILE *process;
  if ((process = popen(command->str, "r")) == NULL) {
    puts(GREEN "Error:" WHITE " Failed to run pacman!!");
    exit(1);
  }

  if ((packageNames = getOutput(process)) == NULL) {
    puts(GREEN "Alert:" WHITE " Nothing to do!!");
    exit(0);
  }
  pclose(process);

  char *returnStr = strdup(packageNames->str);

  freeString(command);
  if (argumentPackages != NULL)
    freeString(argumentPackages);
  freeString(packageNames);

  return returnStr;
}

void createPackageList(packageInfoList *packageList, int toUpdate) {
  char *packageNames = getPackageNames(toUpdate);
  initPackageList(packageList);

  char *packageName = strtok(packageNames, "\n");
  if (packageName == NULL) {
    printf(GREEN "Error: " WHITE "Unable to separate packages from the list!!");
    exit(1);
  }

  // Separates package names, creates packageInfo objects,
  // and inserts them into packageInfoList
  while (packageName != NULL) {
    packageInfo *package;
    initPackageInfo(&package, packageName);
    insertPackage(packageList, package);

    packageName = strtok(NULL, "\n"); // Get the next packageName
  }

  free(packageNames);
}

void execute(char **args) {
  pid_t process;
  process = fork();

  if (process == 0) {
    execvp(args[0], args);
  }
}

void movePackages() {
  char *mvArgs[] = {"sh", "-c",
                    "mv /usr/share/pacx/cache/* /var/cache/pacman/pkg/", NULL};
  execute(mvArgs);
  printf(GREEN "::" WHITE " Successfully moved " GREEN "%d" WHITE
               " packages!!\n",
         packageList.n);
}

void syncPackages() {
  if (!isSudo())
    exit(1);

  // Setup the list of packages
  createPackageList(&packageList, 0);
  printDetails(&packageList);
  puts(""); // Add a new line for separation

  fetchPackages(&packageList);

  movePackages();
  freePackageList(&packageList);

  String *argumentPackages;
  currentArg = 1;
  getArgumentPackages(&argumentPackages);

  int packages = totalArgs - 2;

  char **pacmanArgs = (char **)malloc(sizeof(char *) * (packages + 3));
  pacmanArgs[0] = "pacman";
  pacmanArgs[1] = "-S";
  pacmanArgs[packages + 2] = NULL;

  char *package = strtok(argumentPackages->str, " ");
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

  fetchPackages(&packageList);

  movePackages();
  freePackageList(&packageList);

  char *pacmanArgs[] = {"pacman", "-Su", NULL};
  execvp(pacmanArgs[0], pacmanArgs);
}
