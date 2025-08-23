#include <stdio.h>
#include "AoC64.h"
#if __has_include("input.h")
  #include "input.h"          // user’s private input (gitignored)
#else
  #include "input.example.h"  // checked-in sample input
#endif

inline int8_t inside(const int16_t x, const int16_t y) {
    return x >= target[0][0] && x <= target[0][1] && y >= target[1][0] && y <= target[1][1];
}

inline int8_t overshoot(const int16_t x, const int16_t y) {
    return x > target[0][1] || y < target[1][0];
}

inline int16_t shoot(int16_t vx, int16_t vy) {
    int16_t x = 0, y = 0, maxy = 0;
    for (;;) {
        x += vx;
        y += vy;
        if (y > maxy) maxy = y;
        if (overshoot(x, y)) {return -1;}
        if (inside(x, y)) {return maxy;}
        if (vx) {
            if (vx > 0) vx -= 1; else vx += 1;
        }
        vy -= 1;
    }
}

int main(void) {
    init(17);

    int16_t vx, vy, maxy = 0, hits = 0;
    for (vx = 0; vx <= target[0][1]; ++vx) {
        if ((vx*(vx+1) >> 1) < target[0][0]) continue;
        tick(vx & (uint8_t)7);
        for (vy = target[1][0]; vy < -target[1][0]; ++vy) {
            const int16_t y = shoot(vx, vy);
            if (y >= 0) {
                ++hits;
                if (y > maxy) maxy = y;
            }
        }
    }
    printf("part 1: %d\n", maxy);
    printf("part 2: %d\n", hits);

    finish();
}