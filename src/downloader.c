#include "downloader.h"
#include "urls.h"

#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

extern mirrorTable *repoTable;

void parseDetails(char *summary, packageInfo *package) {
  if (strstr(summary, "DL:") == NULL || strstr(summary, "ETA:") == NULL)
    return;

  char *whitespace_ptr;
  char *token = strtok_r(summary, " ", &whitespace_ptr);

  // Second Part of Summary
  if (token != NULL) {
    token = strtok_r(NULL, " ", &whitespace_ptr);
    if (token == NULL)
      return;

    char *slashPtr;
    char *subToken = strtok_r(token, "/", &slashPtr);
    if (subToken != NULL)
      changePackageInfo(package, 1, strdup(subToken));

    subToken = strtok_r(NULL, "/", &slashPtr);
    if (subToken != NULL) {
      char *paranthesisPtr;
      char *smallerSubToken = strtok_r(subToken, "(", &paranthesisPtr);

      smallerSubToken = strtok_r(NULL, "(", &paranthesisPtr);
      if (smallerSubToken != NULL)
        package->progress = atoi(smallerSubToken);
    }
  }

  // DN: ____ Part
  token = strtok_r(NULL, " ", &whitespace_ptr);
  if (token != NULL)
    token = strtok_r(NULL, " ", &whitespace_ptr);

  while (*token != ':')
    token++;
  token++;

  changePackageInfo(package, 2, strdup(token));
}

void downloadPackage(packageInfo *packageInformation, String *urls) {

  // getMirrors returns only "file" when the package is already present in
  // pacman's cache
  if (strcmp(urls->str, "file") == 0) { // No need to download
    freeString(urls);
    packageInformation->progress = 100;
    packageInformation->notFinished = 0;
    return;
  }

  pid_t processPID;
  int processPipe[2];
  pipe(processPipe);

  processPID = fork();
  if (processPID == 0) { // Child process
    // Process Pipe SetUp
    close(processPipe[0]);   // No need for reading
    dup2(processPipe[1], 1); // Piping stdout
    dup2(processPipe[1], 2); // Piping stderr
    close(processPipe[1]);

    String *aria2cCommand =
        createString("aria2c --continue --optimize-concurrent-downloads -s 5 "
                     "-x 8 --allow-overwrite --file-allocation none "
                     "--summary-interval " UPDATE_INTERVAL " ");
    stringCat(&aria2cCommand, urls);
    stringAppend(&aria2cCommand, " -d ");
    stringAppend(&aria2cCommand, packageInformation->isRepo
                                     ? DB_DIRECTORY
                                     : DOWNLOAD_DIRECTORY);

    char **args = strToArray(aria2cCommand->str);
    freeString(aria2cCommand);

    execvp(args[0], args);
  } else if (processPID > 0) { // Parent Process
    close(processPipe[1]);     // We dont want to write to the pipe
    char buffer[128];
    memset(buffer, 0, sizeof(buffer));

    while ((read(processPipe[0], buffer, sizeof(buffer) - 1)) != 0) {
      unsigned int newlinePos =
          strcspn(buffer, "\n"); // Only want the first line

      // Overflow check
      newlinePos =
          newlinePos >= sizeof(buffer) ? sizeof(buffer) - 1 : newlinePos;
      buffer[newlinePos] = '\0';

      parseDetails(buffer, packageInformation);
    }
    freeString(urls);

    kill(processPID, SIGKILL);
    int status;
    waitpid(processPID, &status, 0);

    packageInformation->progress = 100;

  } else { // Fork error
    printf("Error while downloading the package: %s\n",
           packageInformation->packageName);
  }
}

void *startDownload(void *arg) {
  ((packageInfo *)arg)->downloadStarted = 1;
  String *urls = getUrls(repoTable, ((packageInfo *)arg)->packageName);

  // Removing the prev db files
  if (((packageInfo *)arg)->isRepo) {
    String *fullpath = createString(DB_DIRECTORY "/");
    stringAppend(&fullpath, ((packageInfo *)arg)->packageName);
    remove(fullpath->str);
    freeString(fullpath);
  }

  downloadPackage((packageInfo *)arg, urls);
  ((packageInfo *)arg)->notFinished = 0;
  pthread_exit(NULL);
  return NULL;
}
