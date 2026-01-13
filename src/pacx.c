#include "colors.h"
#include "downloader.h"
#include "urls.h"
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

// Function Prototypes
void printHelp(int currentArg);

typedef struct {
  char *arg;
  void (*operation)(int);
} Argument;

Argument args[] = {{"-h", printHelp}, {"--help", printHelp}};

int main(int argc, char **argv) {
  // Argument Management Start
  if (argc == 1) {
    printHelp(1);
  } else {
    // Iterating through the arguments
    for (int i = 1; i < (sizeof(args) / sizeof(Argument)); i++) {
      if (strcmp(args[i].arg, argv[1]) == 0) {
        args[i].operation(i);
        return 0;
      }
    }
  }
  // Argument Management End
  packageInfo package;
  initPackageInfo(&package, argv[1]);
  downloadPackage(&package);
  return 0;
}

void printHelp(int currentArg) {
  printf(GREEN "Pacx\t" WHITE "A pacman wrapper\n" GREEN "usage:\t" WHITE
               "pacx <operation> [...]\n" GREEN "operations:\n\t" WHITE
               "pacx {-h --help}\n");
}
