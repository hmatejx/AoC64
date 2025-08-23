#define NDEBUG

#include <stdio.h>
#include <string.h>
#include "AoC64.h"
#include "reucpy.h"
#if __has_include("input.h")
  #include "input.h"          // user’s private input (gitignored)
#else
  #include "input.example.h"  // checked-in sample input
#endif

void evolve(uint64_t timers[]) {
    static uint64_t old_timers[9];
    memcpy_reu(old_timers, timers, 9*sizeof(uint64_t));
    for (uint8_t i = 8; i > 0; --i) {
        timers[i - 1] = old_timers[i];
    }
    timers[6] += old_timers[0];
    timers[8] = old_timers[0];
}


uint64_t lanternfish(uint64_t timers[]) {
    unsigned long long res = 0;
    for (uint8_t i = 0; i < 9; ++i) {
        res += timers[i];
    }
    return res;
}


int main(void) {
    init(6);

    for (uint8_t i = 0; i < 80; ++i) {
        evolve(timers);
        tick((i >> 3) & 7);

    }
    printf("part 1: %llu\n", (unsigned long long)lanternfish(timers));

    for (uint8_t i = 0; i < 256-80; ++i) {
        evolve(timers);
        tick((i >> 3) & 7);
    }
    printf("part 2: %llu\n", (unsigned long long)lanternfish(timers));

    finish();
    return 0;
}