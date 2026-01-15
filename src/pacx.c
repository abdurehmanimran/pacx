#include "pacx.h"
#include "colors.h"
#include "downloader.h"
#include "packageinfo.h"
#include "packagelist.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

Argument args[] = {
    {"-h", printHelp}, {"--help", printHelp}, {"-S", syncPackages}};

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
    for (int i = 1; i < (sizeof(args) / sizeof(Argument)); i++) {
      if (strcmp(args[i].arg, argv[1]) == 0) {
        args[i].operation(i, argv);
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

void syncPackages(int currentArg, char **argv) {
  // Setup the list of packages
  retrievePackages(currentArg, totalArgs, argv, &packageList);

  for (int i = 0; i < packageList.n; i++) {
    printf(GREEN "Package [%d]: " WHITE "%s\n", i + 1,
           packageList.packages[i]->packageName);
  }
}
