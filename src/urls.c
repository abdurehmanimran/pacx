#include "urls.h"
#include "colors.h"
#include "str.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

packageURL getPackageURL(char *package) {
  FILE *process;
  String *urls; // getOutput allocates memory itself
  String *command;

  if (package != NULL) { // When package name is provided
    command = createString("pacman -Sddp ");
    stringAppend(&command, package);
  } else {
    printf(RED "Error: " WHITE "Package Name is NULL!!");
    exit(1);
  }

  if ((process = popen(command->str, "r")) == NULL) {
    printf("Failed to get URL");
    exit(1);
  }

  if ((urls = getOutput(process)) == NULL) {
    printf(RED "Error:" WHITE " No URL found for the package: %s\n", package);
    exit(1);
  }
  pclose(process);

  char *returnUrls = strdup(urls->str);
  freeString(urls);
  returnUrls[strcspn(returnUrls, "\0") - 1] = '\0';

  return (packageURL)returnUrls;
}
