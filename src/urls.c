#include "urls.h"
#include "colors.h"
#include "str.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MIRROR_URLS 5

String *getRawMirrors(char *path) {
  String *output;
  int count = 0;
  allocString(&output, 512);
  FILE *mirrorlist = fopen(path, "r");

  if (mirrorlist == NULL) {
    puts("Error: failed to open the mirrorlist !!");
    exit(1);
  }

  char temp_buffer[512];
  memset(temp_buffer, 0, sizeof(temp_buffer));

  while (fgets(temp_buffer, sizeof(temp_buffer), mirrorlist) != NULL &&
         count < MIRROR_URLS) {
    if (strcspn(temp_buffer, "http") == 9) {
      char *url = temp_buffer + 9;

      int line_end_index = strcspn(url, "\n");
      if (line_end_index >= 511)
        line_end_index = 510;
      url[line_end_index] = ' ';

      stringAppend(&output, url);

      memset(temp_buffer, 0, sizeof(temp_buffer));
      count++;
    }
  }

  fclose(mirrorlist);
  return output;
}

char *getPackageURL(char *package) {
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

  return returnUrls;
}
