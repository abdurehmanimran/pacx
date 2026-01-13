#ifndef DOWNLOADER_H
#define DOWNLOADER_H

#define UPDATE_INTERVAL "1"

typedef struct {
  char *packageName;
  char *downloaded;
  char *totalSize;
  char *speed;
  char *progress;
} packageInfo;

#endif // !DOWNLOADER_H
