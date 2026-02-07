#include "downloader.h"
#include "packageinfo.h"
#include "packagelist.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void getDetails(char *summary, packageInfo **package) {
  // Sample Summary: [#243f8f 96KiB/142MiB(0%) CN:1 DL:506KiB ETA:4m48s]

  char *whitespace_ptr;
  char *colon_ptr;

  if (strcspn(summary, "[") == 0) {
    char *token = strtok_r(summary, " ", &whitespace_ptr);
    while (token != NULL) {
      // Fetching the size that has been downloaded
      if (strstr(token, "%)") != NULL) {
        char *downloaded = strdup(token);
        downloaded[strcspn(downloaded, "B/") + 1] = '\0';
        changePackageInfo(*package, 1, downloaded);
        char *totalToken;
        char *totalPtr;
        totalToken = strtok_r(token, "/", &totalPtr);
        if (totalToken != NULL) {
          totalToken = strtok_r(NULL, "/", &totalPtr);
        }
        if (totalToken != NULL) {
          totalToken[strcspn(totalToken, "(")] = '\0';
          changePackageInfo(*package, 2, strdup(totalToken));
        }
      }
      // Getting the speed part from aria2c

      if (strcspn(token, "DL:") == 0) {
        char retoken[strlen(token)];
        int j = 0;
        for (int i = 3; i < strlen(token); i++) {
          retoken[j++] = token[i];
        }
        retoken[strcspn(retoken, "B") + 1] = '\0';
        if (strcmp("nload", retoken) == 0 || strcmp("nload{", retoken) == 0)
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
}

void replaceNewLineChar(char *string, char new) {
  int len = strlen(string);
  for (int i = 0; i < len; i++) {
    if (string[i] == '\n')
      string[i] = new;
  }
}

void downloadPackage(packageInfo *packageInformation) {
  char *url = packageInformation->url;
  // replaceNewLineChar(url, ' ');
  if (url != NULL) {

    if (strcspn(url, "file") == 0) {
      changePackageInfo(packageInformation, 4, strdup("100%"));
    } else {
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
                        "--summary-interval",
                        UPDATE_INTERVAL,
                        url,
                        "-d",
                        DOWNLOAD_DIRECTORY,
                        NULL};
        execvp(args[0], args);
        changePackageInfo(packageInformation, 4, strdup("100%"));
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
        }
      } else {
        printf("Error while downloading the package: %s\n",
               packageInformation->packageName);
      }
    }
  }
}
void *startDownload(void *arg) {
  downloadPackage((packageInfo *)arg);
  ((packageInfo *)arg)->isDownloading = 0;
  return NULL;
}

void createDownloadThreads(pthread_t **threads, packageInfoList *packageList) {
  *threads = (pthread_t *)malloc(sizeof(pthread_t) * packageList->n);

  for (int i = 0; i < packageList->n; i++) {
    pthread_create(&((*threads)[i]), NULL, startDownload,
                   packageList->packages[i]);
  }
  puts("");
}

void waitForDownloadThreads(pthread_t **threads, packageInfoList *packageList) {
  for (int i = 0; i < packageList->n; i++) {
    pthread_join((*threads)[i], NULL);
  }
}
