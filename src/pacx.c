//                      Pacx - A Pacman Wrapper
// A simple hobby project by Abdur Rehman Imran <arehmanimran4@gmail.com>

#include "pacx.h"
#include "args.h"
#include "colors.h"
#include "db.h"
#include "downloader.h"
#include "packageinfo.h"
#include "packagelist.h"
#include "print.h"
#include "str.h"
#include "urls.h"
#include "utils.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

Argument args[] = {
    {"-h", printHelp},       {"--help", printHelp},    {"-S", syncPackages},
    {"-s", syncPackages},    {"--sync", syncPackages}, {"-Su", syncPackages},
    {"-Sy", syncDBs},        {"-Syu", syncDBs},        {"-R", removePackages},
    {"-Rc", removePackages}, {"-Rcs", removePackages}, {"-Rs", removePackages}};

char **arguments;
unsigned int totalArgs;
unsigned int currentArg;
String *pacmanCommand = NULL;
mirrorTable *repoTable = NULL;

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
    // Comparing the given opertaion with the Arguments array
    for (size_t i = 0; i < (sizeof(args) / sizeof(Argument)); i++) {
      if (strcmp(args[i].arg, argv[1]) == 0) {
        currentArg = 1;
        arguments = argv;
        args[i].operation();
        return 0;
      }
    }
    printHelp(); // No other operation performed !!
  }
  // Argument Management End
  return 0;
}

// Level -> 2 To include total stats
// Level -> 1 To exclude total stats
void fetchPackages(packageInfoList *packageList, int level) {
  pthread_t *threads;
  double completedDownloaded = 0;

  packageInfoList *packagesDownloading = malloc(sizeof(packageInfoList));
  initPackageList(packagesDownloading);

  int initThreads = packageList->n >= PARALLEL_DOWNLOADS ? PARALLEL_DOWNLOADS
                                                         : packageList->n;
  int index = initThreads;

  threads = (pthread_t *)malloc(sizeof(pthread_t) * (initThreads));

  for (int i = 0; i < initThreads; i++) {
    insertPackage(packagesDownloading, packageList->packages[i],
                  0); // Total size if not needed
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
          insertPackage(packagesDownloading, packageList->packages[index++],
                        0); // Total size if not needed
          pthread_create(
              &threads[i], NULL, startDownload,
              packagesDownloading->packages[packagesDownloading->n - 1]);
        } else {
        }
      }
    }

    if (packagesDownloading->n <= 0) {
      if (level == 2)
        puts("");
      goto cleanup;
    }

    for (int i = 0; i < packagesDownloading->n; i++) {
      printDownloadInfo(packagesDownloading->packages[i]);
    }

    if (level == 2) {
      String *totalSpeed;
      String *totalDownloaded;
      calcTotalSpeed(packageList, &totalSpeed, &totalDownloaded,
                     completedDownloaded);
      printTotalStats(totalDownloaded->str, totalSpeed->str);

      freeString(totalSpeed);
      freeString(totalDownloaded);
      MOVE_N_LINES_UP(packagesDownloading->n + 1);
    } else
      MOVE_N_LINES_UP(packagesDownloading->n);

    usleep(5000);
  }

cleanup:
  SHOW_CURSOR;
  freePackageList(packagesDownloading);
  free(packagesDownloading);
  free(threads);
}

void createPackageList(packageInfoList *packageList, int toUpdate) {
  char *packageNames = getPackageNames(toUpdate, &pacmanCommand);
  initPackageList(packageList);

  char *packageName = strtok(packageNames, "\n");
  if (packageName == NULL) {
    printf(GREEN "Error: " WHITE "Unable to separate packages from the list!!");
    exit(1);
  }

  // Separates package names, creates packageInfo objects,
  // and inserts them into packageInfoList
  while (packageName != NULL) {
    char *part, *namePtr;
    part = strtok_r(packageName, " ", &namePtr);
    if (!part) {
      printf(GREEN "Error: " WHITE
                   "Unable to separate packages from the list!!");
      exit(1);
    }
    packageInfo *package;
    initPackageInfo(&package, part);

    part = strtok_r(NULL, " ", &namePtr);
    if (!part)
      exit(1);
    double size = atof(part) / (1024 * 1024); // Bytes into MiBs

    insertPackage(packageList, package, size);
    packageName = strtok(NULL, "\n"); // Get the next packageName
  }

  free(packageNames);
}

void syncPackages() {
  char toUpdate = 0;
  if (!isSudo())
    exit(1);

  if (strcmp(arguments[1], "-Su") == 0 || strcmp(arguments[1], "-Syu") == 0) {
    puts(GREEN "::" WHITE " Starting " GREEN "full system " WHITE "update!!");
    toUpdate = 1;
  }

  // Setup the list of packages
  createPackageList(&packageList, toUpdate);
  printDetails(&packageList);

  if (!askYesOrNo()) {
    exit(0);
  }
  puts(""); // Add a new line for separation

  if (repoTable == NULL) { // Setup repoTable for once if not already
    packageInfoList dbList;
    initPackageList(&dbList);
    createDBPackageList(&dbList);
    createMirrorTable(&dbList, &repoTable);
  }

  fetchPackages(&packageList, 2);
  movePackages();
  freePackageList(&packageList); // All use of packageList has ended

  // Making args array
  stringAppend(&pacmanCommand, " --color always");
  replaceInString(&pacmanCommand, "--print-format '%n %s'", " ");
  replaceInString(&pacmanCommand, "   ", "");

  char **pacmanArgs = strToArray(pacmanCommand->str);
  freeString(pacmanCommand); // No need for pacmanCommand now

  execvp(pacmanArgs[0], pacmanArgs);
}

void syncDBs() {
  if (!isSudo())
    exit(1);

  packageInfoList dbList;
  initPackageList(&dbList);
  createDBPackageList(&dbList);

  createMirrorTable(&dbList, &repoTable);
  printf(GREEN "::" WHITE " Syncronizing" GREEN " databases" WHITE " !!\n\n");
  fetchPackages(&dbList, 1);

  freePackageList(&dbList);

  if (strcmp(arguments[currentArg], "-Syu") == 0) {
    puts("");
    syncPackages();
  }
}

void removePackages() {
  if (!isSudo())
    exit(1);

  String *packages = NULL;
  String *pacmanCommand = createString("sudo pacman ");
  // appending -R / -Rc / -Rcs / ...
  stringAppend(&pacmanCommand, arguments[1]); // same as currentArg
  stringAppend(&pacmanCommand, " ");

  getArgumentPackages(&packages);

  if (packages) {
    stringCat(&pacmanCommand, packages);
    freeString(packages);
  } else {
    printf(RED "Error: " WHITE "no package was provided to remove !!\n");
    exit(1);
  }

  char **pacmanArgs = strToArray(pacmanCommand->str);
  printf(GREEN ":: " WHITE "Requesting " GREEN "pacman" WHITE
               " to kindly remove the package(s) !!\n");

  if (pacmanCommand) {
    execvp(pacmanArgs[0], pacmanArgs);
    freeString(pacmanCommand);
  }
}
