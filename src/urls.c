#include "urls.h"
#include "colors.h"
#include "packageattr.h"
#include "str.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MIRROR_URLS 5

String *getRawMirrors(char *path, packageAttr *attrs) {
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
      url[line_end_index] = '\0';

      stringAppend(&output, url);
      fillURLPlaceholders(&output, attrs);
      stringAppend(&output, "/");
      stringAppend(&output, attrs->fileName);
      stringAppend(&output, " ");

      memset(temp_buffer, 0, sizeof(temp_buffer));
      count++;
    }
  }

  output->str[output->size - 1] = '\0';
  fclose(mirrorlist);
  return output;
}

void fillURLPlaceholders(String **urls, packageAttr *attrs) {
  if (!urls || (*urls)->size == 0)
    return;

  char archBuff[24];
  memset(archBuff, 0, sizeof(archBuff));
  char *archStart = strstr((*urls)->str, "$arch");

  for (int i = 0; *archStart != '/' && *archStart != 0; archStart++)
    archBuff[i++] = *archStart;

  replaceInString(urls, archBuff, attrs->arch);
  replaceInString(urls, "$repo", attrs->repo);
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

packageAttr *getPackageAttr(char *package) {
  packageAttr *pkgAttr = initPackageAttr();
  FILE *process;
  String *pacmanOut = NULL; // getOutput allocates memory itself
  String *command = NULL;

  if (package != NULL) { // When package name is provided
    command = createString("pacman -Sddp ");
    stringAppend(&command, package);
    stringAppend(&command, " --print-format \"%a %r %f %l\"");
  } else {
    printf(RED "Error: " WHITE "Package Name is NULL!!");
    goto nullCleanUp;
  }

  if ((process = popen(command->str, "r")) == NULL) {
    printf("Failed to get URL");
    goto nullCleanUp;
  }

  if ((pacmanOut = getOutput(process)) == NULL) {
    printf(RED "Error:" WHITE " No URL found for the package: %s\n", package);
    pclose(process);
    goto nullCleanUp;
  }
  pclose(process);

  char *i, *save;
  i = strtok_r(pacmanOut->str, " ", &save);
  if (!i)
    goto nullCleanUp;
  pkgAttr->arch = strdup(i);
  i = strtok_r(NULL, " ", &save);
  if (!i)
    goto nullCleanUp;
  pkgAttr->repo = strdup(i);
  i = strtok_r(NULL, " ", &save);
  if (!i)
    goto nullCleanUp;
  pkgAttr->fileName = strdup(i);

  i = strtok_r(NULL, " ", &save);
  if (!i)
    goto nullCleanUp;
  i[strcspn(i, "\n")] = 0;
  pkgAttr->url = strdup(i);

cleanup:
  if (command)
    freeString(command);
  if (pacmanOut) {
    freeString(pacmanOut);
  }
  return pkgAttr;

nullCleanUp:
  if (pkgAttr)
    free(pkgAttr);
  pkgAttr = NULL;
  goto cleanup;
}

String *getMirrors(char *package) {
  String *mirrors = NULL;
  packageAttr *pkgAttr = NULL;
  String *listPath = NULL;

  pkgAttr = getPackageAttr(package);
  if (!pkgAttr)
    goto cleanup;

  if (strstr(pkgAttr->url, "file://"))
    goto cleanup;

  listPath = getMirrorListPath(pkgAttr);
  mirrors = getRawMirrors(listPath->str, pkgAttr);

cleanup:
  if (pkgAttr != NULL)
    freePackageAttr(pkgAttr);
  if (listPath != NULL)
    freeString(listPath);

  return mirrors;
}
