#include "colors.h"
#include <stdio.h>
#include <wchar.h>

void printBars(int n) {
  for (int i = 0; i < n; i++)
    printf("%s", "\u254D");
  for (int i = 0; i < 100 - n; i++)
    printf(GREEN "%s" WHITE, "\u2501");
}

int main() { printBars(10); }
