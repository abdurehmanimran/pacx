#include "downloader.h"
#include "colors.h"
#include "packageinfo.h"
#include "packagelist.h"
#include "urls.h"
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

void parseDetails(char *summary, packageInfo *package) {
  if (strstr(summary, "ETA:") == NULL)
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

      if (smallerSubToken != NULL)
        changePackageInfo(package, 2, strdup(smallerSubToken));

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

  changePackageInfo(package, 3, strdup(token));
}

void downloadPackage(packageInfo *packageInformation) {
  char *url = getPackageURL(packageInformation->packageName);
  if (!url) {
    printf(RED "Error:" WHITE " failed to fetch the url !!\n");
    return;
  }

  if (strcspn(url, "file") == 0) { // No need to download
    free(url);
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

    // Executing aria2c
    char *args[] = {"aria2c",
                    "--continue",
                    "--optimize-concurrent-downloads",
                    "-s",
                    "6",
                    "-x",
                    "8",
                    "--file-allocation",
                    "none",
                    "--summary-interval",
                    UPDATE_INTERVAL,
                    url,
                    "-d",
                    DOWNLOAD_DIRECTORY,
                    NULL};
    execvp(args[0], args);
  } else if (processPID > 0) { // Parent Process
    // Reading the stdout of the child process
    close(processPipe[1]); // We dont want to write to the pipe

    char buffer[128];

    while ((read(processPipe[0], buffer, sizeof(buffer) - 1)) != 0) {
      unsigned int newlinePos =
          strcspn(buffer, "\n"); // Only want the first line

      // Overflow check
      newlinePos =
          newlinePos >= sizeof(buffer) ? sizeof(buffer) - 1 : newlinePos;
      buffer[newlinePos] = '\0';

      char info[strlen(buffer) + 1];
      strncpy(info, buffer, strlen(buffer) + 1);
      parseDetails(info, packageInformation);
    }

    kill(processPID, SIGKILL);
    int status;
    waitpid(processPID, &status, 0);

    packageInformation->progress = 100;
    free(url);
  } else { // Fork error
    printf("Error while downloading the package: %s\n",
           packageInformation->packageName);
  }
}

void *startDownload(void *arg) {
  ((packageInfo *)arg)->downloadStarted = 1;
  downloadPackage((packageInfo *)arg);
  ((packageInfo *)arg)->notFinished = 0;
  pthread_exit(NULL);
  return NULL;
}
