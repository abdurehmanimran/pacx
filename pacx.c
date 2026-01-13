#include "colors.h"
#include "urls.h"
#include <stdio.h>
#include <stdlib.h>
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
        break;
      }
    }
  }
  // Argument Management End
  char *packages[] = {"neofetch", "vim", "fastfetch", "linux"};
  URLs urls;
  urls = getURLs(4, packages);

  for (int i = 0; i < (sizeof(packages) / sizeof(char *)); i++) {
    printf(GREEN "URL (%d): " WHITE "%s\n", i + 1, urls[i]);
  }

  return 0;
}

void printHelp(int currentArg) {
  printf(GREEN "Pacx\t" WHITE "A pacman wrapper\n" GREEN "usage:\t" WHITE
               "pacx <operation> [...]\n" GREEN "operations:\n\t" WHITE
               "pacx {-h --help}\n");
}
