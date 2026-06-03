#include "progress.h"
#include <stdio.h>

void printProgress(double progress, int width) {
  int dashes = (progress / 100) * width;

  for (int i = 0; i < dashes; i++)
    printf("%s", "\u2501");
  for (int i = 0; i < width - dashes; i++)
    printf("%s", "\u254D");
}
