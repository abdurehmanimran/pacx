#include "args.h"
#include "colors.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int checkArgument(const char *toFind) {
  unsigned int index = currentArg;

  while (index < totalArgs)
    if (toFind && strstr(arguments[index++], toFind))
      return 1;

  return 0;
}

void getArgumentPackages(String **buffer) {
  unsigned int index = currentArg;
  allocString(buffer, 1024);
  index++;

  if (index >= totalArgs) {
    freeString(*buffer);
    *buffer = NULL;
  }

  while (index < totalArgs) {
    if (arguments[index][0] == '-') {
      index++;
      continue;
    }

    stringAppend(buffer, arguments[index++]);
    stringAppend(buffer, " ");
  }
}

void getPackagesToIgnore(String **buffer) {
  unsigned int index = currentArg;

  while (strstr(arguments[index], "--ignore") == NULL) {
    index++;
    if (index >= totalArgs)
      return;
  }

  index++;
  if (index < totalArgs) // Check if there is any package name after --ignore
    allocString(buffer, 1024);
  else
    return;

  while (index < totalArgs) {
    if (arguments[index][0] == '-')
      break;

    stringAppend(buffer, "--ignore ");
    stringAppend(buffer, arguments[index++]);
    stringAppend(buffer, " ");
  }
}

// Returns a malloced list of packages each on a separate line
char *getPackageNames(int toUpdate, String **command) {
  String *argumentPackages = NULL;
  String *toIgnore = NULL;

  getPackagesToIgnore(&toIgnore);

  String *packageNames;

  if (toUpdate) {
    *command = createString("pacman -Su --print-format '%n %s' ");
    if (toIgnore) {
      stringCat(command, toIgnore);
      freeString(toIgnore);
    }
  } else {
    *command = createString("pacman -S ");

    getArgumentPackages(&argumentPackages);

    if (argumentPackages == NULL) {
      puts(GREEN "Alert:" WHITE " Nothing to do!!");
      exit(0);
    }

    stringCat(command, argumentPackages);
    if (checkArgument("--needed") && !toUpdate)
      stringAppend(command, " --needed ");

    stringAppend(command, " --print-format '%n %s' ");
    if (toIgnore) {
      stringCat(command, toIgnore);
      freeString(toIgnore);
    }
  }

  FILE *process;
  if ((process = popen((*command)->str, "r")) == NULL) {
    puts(GREEN "Error:" WHITE " Failed to run pacman!!");
    exit(1);
  }

  if ((packageNames = getOutput(process)) == NULL) {
    puts(GREEN "Alert:" WHITE " Nothing to do!!");
    exit(0);
  }
  pclose(process);

  char *returnStr = strdup(packageNames->str);

  if (argumentPackages != NULL)
    freeString(argumentPackages);
  freeString(packageNames);

  return returnStr;
}
