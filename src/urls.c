#include "urls.h"
#include "colors.h"
#include "str.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

packageURL getPackageURL(char *package, int ignoreDependencies) {
  FILE *process;
  // char urls[5024];
  String *urls;
  allocString(&urls, 5124);
  // char command[128];
  String *command;

  if (package != NULL) { // When package name is provided
    command = createString("pacman -Sddp ");
    stringAppend(&command, package);
    // strcpy(command, "pacman -Sddp ");
    // strcat(command, package);
  } else {
    printf(RED "Error: " WHITE "Package Name is NULL!!");
    exit(1);
  }

  if ((process = popen(command->str, "r")) == NULL) {
    printf("Failed to get URL");
    exit(1);
  }

  char buffer[1025];
  memset(buffer, 0, sizeof(buffer));

  long unsigned int bytes =
      fread(buffer, sizeof(char), sizeof(buffer) - 1, process);

  if (bytes == 0) {
    printf(RED "Error:" WHITE " No URL found for the package: %s\n", package);
    exit(1);
  }

  while (bytes > 0) {
    stringAppend(&urls, buffer);
    // strcat(urls, buffer);

    if (bytes < sizeof(buffer) - 1) {
      if (feof(process))
        break;
    }

    memset(buffer, 0, sizeof(buffer));
    bytes = fread(buffer, sizeof(char), sizeof(buffer) - 1, process);
  }
  char *returnUrls = strdup(urls->str);
  freeString(urls);
  returnUrls[strcspn(returnUrls, "\0") - 1] = '\0';

  return (packageURL)returnUrls;
}
