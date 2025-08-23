#include "AoC64.h"
#include "stack.h"
#if __has_include("input.h")
  #include "input.h"          // user’s private input (gitignored)
#else
  #include "input.example.h"  // checked-in sample input
#endif


struct Cucumber {
    uint8_t x;
    uint8_t y;
};

Stack<Cucumber, uint16_t, 5000> cucumbers;


inline const uint8_t east(const uint8_t x) {
    uint8_t nx = x + 1;
    if (nx == dimx)
        return 0;
    return nx;
}


inline const uint8_t down(const uint8_t y) {
    uint8_t ny = y + 1;
    if (ny == dimy)
        return 0;
    return ny;
}


int8_t step() {
    // east facing cucumbers
    cucumbers.clear();
    for (uint8_t i = 0; i < dimy; ++i) {
        for (uint8_t j = 0; j < dimx; ++j) {
            if (input[i][j] != '>')
                continue;
            if (input[i][east(j)] == '.')
                cucumbers.push(Cucumber {j, i});
        }
    }
    const uint16_t m1 = cucumbers.size();
    for (uint16_t k = 0; k < m1; ++k) {
        const Cucumber& c = cucumbers.arr[k];
        input[c.y][c.x] = '.';
        input[c.y][east(c.x)] = '>';
    }
    // down facing cucumbers
    cucumbers.clear();
    for (uint8_t i = 0; i < dimy; ++i) {
        for (uint8_t j = 0; j < dimx; ++j) {
            if (input[i][j] != 'v')
                continue;
            if (input[down(i)][j] == '.')
                cucumbers.push(Cucumber {j, i});
        }
    }
    const uint16_t m2 = cucumbers.size();
    for (uint16_t k = 0; k < m2; ++k) {
        const Cucumber& c = cucumbers.arr[k];
        input[c.y][c.x] = '.';
        input[down(c.y)][c.x] = 'v';
    }

    return m1 > 0 || m2 > 0;
}


int main(void) {
    init(25);

    uint8_t t = 0;
    uint16_t steps = 0;
    do {
        tick(t++ & (uint8_t)7);
        ++steps;
    } while (step());
    printf("part 1: %d\n", steps);

    finish();
}