#include <stdio.h>
#include <string.h>
#include "AoC64.h"
#if __has_include("input.h")
  #include "input.h"          // user’s private input (gitignored)
#else
  #include "input.example.h"  // checked-in sample input
#endif


int main(void) {
    init(1);

    int res1 = 0, res2 = 0;
    const int16_t n = sizeof(lines) / sizeof(lines[0]);

    int prev = lines[0], next = 0;
    for (int i = 0; i < n; ++i) {
        next = lines[i];
        res1 = next > prev ? res1 + 1 : res1;
        prev = next;
        tick((i >> 7) & 7);
    }
    printf("part 1: %d\n", res1);

    prev = lines[0] + lines[1] + lines[2];
    for (int i = 1; i < n-2; ++i) {
        next = lines[i] + lines[i+1] + lines[i+2];
        res2 = next > prev ? res2 + 1 : res2;
        prev = next;
        tick((i >> 7) & 7);
    }
    printf("part 2: %d\n", res2);

    finish();
}