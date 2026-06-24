#include "urls.h"
#include "colors.h"
#include "str.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>

packageURL getPackageURL(char *package) {
  FILE *process;
  char *returnUrls = NULL;
  String *urls; // getOutput allocates memory itself
  String *command;

  if (package != NULL) { // When package name is provided
    command = createString("pacman -Sddp ");
    stringAppend(&command, package);
  } else {
    printf(RED "Error: " WHITE "Package Name is NULL!!");
    return NULL;
  }

  if ((process = popen(command->str, "r")) == NULL) {
    printf("Failed to get URL");
    return NULL;
  }

  if ((urls = getOutput(process)) == NULL) {
    printf(RED "Error:" WHITE " No URL found for the package: %s\n", package);
    pclose(process);
    return NULL;
  }
  pclose(process);

  returnUrls = strdup(urls->str);
  freeString(command);
  freeString(urls);
  returnUrls[strcspn(returnUrls, "\n")] = '\0';

  return (packageURL)returnUrls;
}
