#include "downloader.h"
#include "colors.h"
#include "urls.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Defination of packageInfo:
// typedef struct {
//   char *packageName;
//   char *downloaded;
//   char *totalSize;
//   char *progress;
// } packageInfo;

void initPackageInfo(packageInfo *package, char *name) {
  package->packageName = name;
  package->downloaded = strdup("0B");
  package->progress = strdup("0%");
  package->speed = strdup("0B");
  package->totalSize = strdup("0B");
}

void freePackageInfo(packageInfo *package) {
  free(package->downloaded);
  free(package->speed);
  free(package->totalSize);
  free(package->progress);
}

void changePackageInfo(packageInfo *package, int opt, char *newValue) {
  switch (opt) {
  case 1:
    free(package->downloaded);
    package->downloaded = newValue;
    break;
  case 2:
    free(package->totalSize);
    package->totalSize = newValue;
    break;
  case 3:
    free(package->speed);
    package->speed = newValue;
    break;
  case 4:
    free(package->progress);
    package->progress = newValue;
    break;
  }
}

void getDetails(char *summary, packageInfo **package) {
  // Sample Summary: [#243f8f 96KiB/142MiB(0%) CN:1 DL:506KiB ETA:4m48s]

  char *whitespace_ptr;
  char *colon_ptr;
  char *token = strtok_r(summary, " ", &whitespace_ptr);
  while (token != NULL) {

    // Fetching the size that has been downloaded
    if (strstr(token, "%)") != NULL) {
      char *downloaded = strdup(token);
      downloaded[strcspn(downloaded, "B/") + 1] = '\0';
      changePackageInfo(*package, 1, downloaded);
    }
    // Getting the speed part from aria2c

    if (strcspn(token, "DL:") == 0) {
      char retoken[strlen(token)];
      int j = 0;
      for (int i = 3; i < strlen(token); i++) {
        retoken[j++] = token[i];
      }
      retoken[strcspn(retoken, "B") + 1] = '\0';
      if (strcmp("nload", retoken) == 0)
        changePackageInfo(*package, 3, strdup("0B"));
      // (*package)->speed = strdup("0B");
      else
        changePackageInfo(*package, 3, strdup(retoken));
      // (*package)->speed = strdup(retoken);
    }
    // else if (strcspn(token, ""))
    token = strtok_r(NULL, " ", &whitespace_ptr);
  }
}

void downloadPackage(packageInfo *packageInformation) {
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
    char *url = getPackageURL(packageInformation->packageName);

    char *args[] = {"aria2c",
                    "--continue",
                    "--optimize-concurrent-downloads",
                    "--summary-interval",
                    UPDATE_INTERVAL,
                    url,
                    "-d",
                    "./",
                    NULL};
    execvp(args[0], args);
    free(url);
  } else if (processPID > 0) {
    // Parent Process
    // Reading the stdout of the child process
    close(processPipe[1]); // We dont want to write to the pipe
    char buffer[512];
    while ((read(processPipe[0], buffer, sizeof(buffer))) != 0) {
      buffer[strcspn(buffer, "\n")] = '\0';
      char info[strlen(buffer) + 1];
      strncpy(info, buffer, strlen(buffer) + 1);
      getDetails(info, &packageInformation);
      printf("\r" GREEN "Speed: " WHITE "%s\t " GREEN "Downloaded: " WHITE "%s",
             packageInformation->speed, packageInformation->downloaded);
      fflush(stdout);
      fflush(stderr);
    }
  } else {
    printf("Error while downloading the package: %s\n",
           packageInformation->packageName);
  }
}
