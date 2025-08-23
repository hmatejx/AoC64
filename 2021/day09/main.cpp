#include <stdio.h>
#include <string.h>
#include "AoC64.h"
#include "queue.h"
#if __has_include("input.h")
  #include "input.h"          // user’s private input (gitignored)
#else
  #include "input.example.h"  // checked-in sample input
#endif

struct Point {
    int8_t y;
    int8_t x;

    const Point up()    const { return Point {.y = (int8_t)(y-1), .x = x}; }
    const Point down()  const { return Point {.y = (int8_t)(y+1), .x = x}; }
    const Point left()  const { return Point {.y = y, .x = (int8_t)(x-1)}; }
    const Point right() const { return Point {.y = y, .x = (int8_t)(x+1)}; }
};

// global vars for flood fill
constexpr uint16_t MAX_SIZE = 255;
Queue<Point, uint8_t, MAX_SIZE> front;
static int8_t visited[dimy][dimx] = {{}};


inline void explore(const Point p) {
    if (p.y < 0 || p.x < 0 || p.y >= dimy || p.x >= dimx || heightmap[p.y][p.x] == '9' || visited[p.y][p.x])
        return;
    visited[p.y][p.x] = 1;
    front.push(p);
}

int16_t flood_fill(const Point p0) {
    int16_t count = 0;
    front.push(p0);
    visited[p0.y][p0.x] = 1;
    while (front.size()) {
        const Point p = front.pop();
        ++count;
        explore(p.up()); explore(p.down()); explore(p.left()); explore(p.right());
    }
    return count;
}

int main(void) {
    init(9);

    int16_t risk = 0;
    int32_t top3[3] = {};
    for (int8_t i = 0; i < dimy; ++i) {
        tick(i  & 7);
        for (int8_t j = 0; j < dimx; ++j) {
            const char h = heightmap[i][j];
            if ((i == 0        || heightmap[i-1][j] > h) &&
                (i == dimy - 1 || heightmap[i+1][j] > h) &&
                (j == 0        || heightmap[i][j-1] > h) &&
                (j == dimx - 1 || heightmap[i][j+1] > h)) {
                    risk += h - '0' + 1;
                    if (!visited[i][j]) {
                        const int16_t basin = flood_fill(Point {.y=i, .x=j});
                        if (basin > top3[0]) {
                            top3[2] = top3[1]; top3[1] = top3[0]; top3[0] = basin;
                        } else if (basin > top3[1]) {
                            top3[2] = top3[1]; top3[1] = basin;
                        } else if (basin > top3[2]) {
                            top3[2] = basin;
                        }
                    }
                }
        }
    }
    printf("part 1: %d\n", risk);
    printf("part 2: %ld\n", top3[0]*top3[1]*top3[2]);

    finish();
}
