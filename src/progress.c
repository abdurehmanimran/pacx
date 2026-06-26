#include "progress.h"
#include "colors.h"
#include <stdio.h>

void printProgress(double progress, int width) {
  int dashes = (progress / 100) * width;

  if (progress == 0)
    printf(BLACK);

  for (int i = 0; i < dashes; i++)
    printf("%s", BAR);

  if (progress > 0 && progress < 100) {
    printf(BLUE "%s" BLACK, SPLIT_BAR);
    dashes++;
  }

  for (int i = 0; i < width - dashes; i++)
    printf("%s", BAR);
}
