#include "urls.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

packageURL getPackageURL(char *package) {
  FILE *process;
  char command[256];
  strcpy(command, "sudo pacman -Sp ");
  strcat(command, package);

  if ((process = popen(command, "r")) == NULL) {
    printf("Failed to get URL");
    exit(1);
  }
  char buffer[1024];
  if ((fgets(buffer, 1024, process) == NULL)) {
    printf("Error: failed to get url!");
    exit(1);
  }
  buffer[strcspn(buffer, "\n")] = '\0';
  return (packageURL)strdup(buffer);
}
