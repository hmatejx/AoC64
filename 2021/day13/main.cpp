#include <stdio.h>
#include <stdlib.h>
#include "AoC64.h"
#include "hashset.h"
#include "pearson.h"
#if !__has_include("input.h")
  #include "input.h"          // user’s private input (gitignored)
#else
  #include "input.example.h"  // checked-in sample input
#endif

constexpr int16_t MAX_SIZE = 1 << 10;  // needs to be power of 2 for MOD macro
#define MOD(x) ((x) & (MAX_SIZE - 1))


struct Point {
    int16_t x;
    int16_t y;
    const uint16_t hash() const { return MOD(this->x*31 + this->y); }
};
bool operator==(const Point& lhs, const Point& rhs) { return lhs.x == rhs.x && lhs.y == rhs.y; }

// global hash set
// 1KB entries = 5KB memory used (4KB for data, 1KB for markers)
HashSet<Point, int16_t, MAX_SIZE> hs;

void foldx(const int16_t x) {
    Point point;
    for (int16_t i = 0; i < MAX_SIZE; ++i) {
        tick((i >> 6) & (uint8_t)7);
        if (!hs.isOccupied(i) || hs.data[i].x <= x)
            continue;
        point = hs.data[i];
        hs.remove(point);
        point.x = 2*x - point.x;
        hs.insert(point);
    }
}

void foldy(const int16_t y) {
    Point point;
    for (int16_t i = 0; i < MAX_SIZE; ++i) {
        tick((i >> 6) & (uint8_t)7);
        if (!hs.isOccupied(i) || hs.data[i].y <= y)
            continue;
        point = hs.data[i];
        hs.remove(point);
        point.y = 2*y - point.y;
        hs.insert(point);
    }
}

void fold(const int8_t i) {
    const char* eq = strchr(folds[i], '=');
    if (*(eq-1) == 'x')
        foldx(atoi(eq+1));
    else
        foldy(atoi(eq+1));
}

int main(void) {
    init(13);

    hs.clear();
    for (int16_t i = 0; i < n_dots; ++i) {
        tick((i >> 6) & (uint8_t)7);
        hs.insert(*(Point *)(&dots[i]));
    }
    fold(0);
    int16_t visible = 0;
    for (int16_t i = 0; i < MAX_SIZE; ++i) {
        if (hs.isOccupied(i))
            ++visible;
    }
    printf("part 1: %d\n", visible);

    // part 2
    for (int8_t i = 1; i < n_folds; ++i) {
        fold(i);
    }
    printf("part 2:\n");
    for (int8_t i = 0; i < 6; ++i) {
        for (int8_t j = 0; j < 39; ++j) {
            Point p = {.x = j, .y = i};
            if (hs.find(p) != -1) printf("#"); else printf(".");
        }
        printf("\n");
    }

    finish();
}