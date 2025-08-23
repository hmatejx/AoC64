#include <stdio.h>
#include <string.h>
#include "AoC64.h"
#include "stack.h"
#include "reucpy.h"
#if __has_include("input.h")
  #include "input.h"          // user’s private input (gitignored)
#else
  #include "input.example.h"  // checked-in sample input
#endif

struct Point {
    int8_t y;
    int8_t x;
};

// global stack
Stack<Point, uint8_t, 128> stack;

// global variables for flood fill
static int8_t flashed[dimy][dimx]; // tracking octopi that already flashed
static int8_t marked[dimy][dimx];  // to prevent multiple instances of the same octopus on the stack
int8_t flash() {
    int8_t flashes = 0;
    memset_reu(0, (uintptr_t)flashed, n_octopi);
    memset_reu(0, (uintptr_t)marked, n_octopi);
    static const int8_t neighbors[8][2] = {{-1, -1}, {0, -1}, {1, -1}, {-1, 0}, {1, 0}, {-1, 1}, {0, 1}, {1, 1}};
    // first, increase energy level of all by 1
    for (int8_t i = 0; i < dimy; ++i) {
        for (int8_t j = 0; j < dimx; ++j) {
            ++energy[i][j];
        }
    }
    // next, walk over the grid and find an octopus that will flash
    for (int8_t i = 0; i < dimy; ++i) {
        for (int8_t j = 0; j < dimx; ++j) {
            // if an octopus can flash, push it onto the stack
            if (energy[i][j] > '9' && !flashed[i][j]) {
                stack.clear();
                stack.push(Point {.y = i, .x = j});
                // repeat while the stack is not empty
                while (stack.size()) {
                    const Point p = stack.pop();
                    if (flashed[p.y][p.x])
                        continue;
                    // this octopus flashes
                    ++flashes;
                    flashed[p.y][p.x] = 1;
                    energy[p.y][p.x] = '0';
                    // neighboring octopuses that have not yet flashed increase their energy by 1
                    for (int8_t k = 0; k < 8; ++k) {
                        const int8_t y1 = p.y + neighbors[k][0], x1 = p.x + neighbors[k][1];
                        if (y1 < 0 || y1 >= dimy || x1 < 0 || x1 >= dimx || flashed[y1][x1] || marked[y1][x1])
                            continue;
                        ++energy[y1][x1];
                        // if their new energy exceeds 9, push them onto the stack
                        if (energy[y1][x1] > '9') {
                            marked[y1][x1] = 1;
                            stack.push(Point {.y = y1, .x = x1});
                        }
                    }
                }
            }
        }
    }
    return flashes;
}

void display(int16_t i) {
    printf("after step %d:\n", i);
    for (int8_t i = 0; i < dimy; ++i) {
        for (int8_t j = 0; j < dimx; ++j) {
            printf("%c", energy[i][j]);
        }
        printf("\n");
    }
}

int main(void) {
    init(11);
    reuset(0, 0, 1);

    int16_t i;
    int16_t res = 0;
    for (i = 0; i < 100; ++i) {
        tick((i >> 1) & (uint8_t)7);
        res += flash();
    }
    printf("part 1: %d\n", res);

    int8_t flashes;
    do {
        tick((i >> 1) & (uint8_t)7);
        flashes = flash();
        ++i;
    } while (flashes < n_octopi);
    //
    printf("part 2: %d\n", i);

    finish();
}
