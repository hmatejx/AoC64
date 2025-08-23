#include <cstdio>
#include <string.h>
#include "../lib/AoC64.h"
#include "input.h"

constexpr uint8_t n_bingos = sizeof(bingos)/sizeof(bingos[0]);
constexpr uint8_t n_numbers = sizeof(numbers)/sizeof(numbers[0]);
uint8_t win[n_bingos];
uint8_t when[n_bingos];
uint8_t hit[n_bingos][5][5] = {{{}}};

void mark(const uint8_t b, const uint8_t n) {
    for (uint8_t i = 0; i < 5; ++i) {
        for (uint8_t j = 0; j < 5; ++j) {
            if (bingos[b][i][j] == n) {
                hit[b][i][j] = 1;
                return;
            }
        }
    }
}

uint16_t score(const uint8_t b, const uint8_t n) {
    uint16_t s = 0;
    for (uint8_t i = 0; i < 5; ++i) {
        for (uint8_t j = 0; j < 5; ++j) {
            if (hit[b][i][j] == 0) {
                s += bingos[b][i][j];
            }
        }
    }
    return s * n;
}

uint8_t check_bingo(const uint8_t b) {
    for (uint8_t j = 0; j < 5; ++j) {
        uint8_t sum = 0;
        for (uint8_t k = 0; k < 5; ++k) {
            sum += hit[b][j][k];
        }
        if (sum == 5) return 1;
        sum = 0;
        for (uint8_t k = 0; k < 5; ++k) {
            sum += hit[b][k][j];
        }
        if (sum == 5) return 1;
    }
    return 0;
}

void play_bingo() {
    for (uint8_t i = 0; i < n_numbers; ++i) {
        tick(i & (uint8_t)7);
        for (int b = 0; b < n_bingos; ++b) {
            if (win[b]) continue;
            mark(b, numbers[i]);
            win[b] = check_bingo(b);
            when[b] = i;
        }
    }
}

int main(void) {
    init(4);

    play_bingo();
    uint8_t min = n_numbers;
    uint8_t max = 0;
    uint8_t winning, loosing;
    for (uint8_t b = 0; b < n_bingos; ++b) {
        if (when[b] < min) { min = when[b]; winning = b; }
        if (when[b] > max) { max = when[b]; loosing = b; }
    }

    printf("part 1: %u\n",score(winning, numbers[when[winning]]));
    printf("part 2: %u\n", score(loosing, numbers[when[loosing]]));

    finish();
}
