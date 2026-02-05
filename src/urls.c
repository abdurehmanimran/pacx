#include "urls.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

packageURL getPackageURL(char *package, int ignoreDependencies) {
  FILE *process;
  char urls[5024];
  char command[128];

  if (package != NULL) { // When package name is provided
    if (ignoreDependencies)
      strcpy(command, "pacman -Sddp ");
    else
      strcpy(command, "pacman -Sp ");
    strcat(command, package);
  } else {
    printf("Error: Package Name is NULL!!");
    exit(1);
  }

  if ((process = popen(command, "r")) == NULL) {
    printf("Failed to get URL");
    exit(1);
  }
  char buffer[1024];

  if ((fgets(buffer, 1024, process) == NULL)) {
    printf("Error: failed to get url!");
    exit(1);
  } else {
    strcpy(urls, buffer);
  }

  while (fgets(buffer, 1024, process) != NULL) {
    strcat(urls, buffer);
  }
  urls[strcspn(urls, "\0") - 1] = '\0';
  return (packageURL)strdup(urls);
}
