#include <stdio.h>
#include <string.h>
#include "AoC64.h"
#if __has_include("input.h")
  #include "input.h"          // user’s private input (gitignored)
#else
  #include "input.example.h"  // checked-in sample input
#endif

inline int32_t min(const int32_t a, const int32_t b) { return a > b ? b : a; }

int main(void) {
    init(7);

    // part 1
    int32_t res1 = 1000000, m1 = 0;
    for (int16_t c = 0; c < n_crabs; ++ c) {
        tick(c & (uint8_t)7);
        int32_t fuel = 0;
        const int16_t pos = crabs[c];
        m1 += pos;
        for (int16_t j = 0; j < n_crabs; ++j) {
            fuel += crabs[j] > pos ? crabs[j] - pos : pos - crabs[j];
        }
        if (fuel < res1)
            res1 = fuel;
    }

    // part 2
    int32_t res21 = 0, res22 = 0;
    m1 = m1 / n_crabs;
    uint32_t m2 = m1 + 1;
    for (int16_t i = 0; i < n_crabs; ++i) {
        int32_t d1 = crabs[i] - m1;
        int32_t d2 = crabs[i] - m2;
        if (d1 < 0)
            d1 = -d1;
        if (d2 < 0)
            d2 = -d2;
        res21 += d1 * (d1 + 1);
        res22 += d2 * (d2 + 1);
    }

    printf("part 1: %ld\n", res1);
    printf("part 2: %ld\n", min(res21, res22) / 2);

    finish();
}