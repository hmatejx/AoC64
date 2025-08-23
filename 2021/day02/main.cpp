#include <cstdlib>
#include <string.h>
#include "AoC64.h"
#if __has_include("input.h")
  #include "input.h"          // user’s private input (gitignored)
#else
  #include "input.example.h"  // checked-in sample input
#endif


struct Instruction {
    const char* direction[input_len];
    uint8_t distance[input_len];
} instructions;


void parse_input() {
    for (uint16_t i = 0; i < input_len; ++i) {
        instructions.direction[i] = &(input[i][0]);
        instructions.distance[i] = (uint8_t)atoi(strchr(input[i], ' '));
    }
}


uint32_t navigate(const bool part2 = false) {
    uint32_t d = 0, x = 0, aim = 0;
    for (uint16_t i = 0; i < input_len; ++i) {
        tick((i >> 7) & 7);
        if (*(instructions.direction[i]) == 'f') {
            x += instructions.distance[i];
            if (part2) { d += aim*instructions.distance[i]; }
        } else if (*(instructions.direction[i]) == 'd') {
            if (part2) { aim += instructions.distance[i]; } else { d += instructions.distance[i]; }
        } else if (*(instructions.direction[i]) == 'u') {
            if (part2) { aim -= instructions.distance[i]; } else { d -= instructions.distance[i]; }
        }
    }
    return x*d;
}

int main(void) {
    init(2);

    parse_input();

    printf("part 1: %lu\n", navigate());
    printf("part 2: %lu\n", navigate(true));

    finish();
    return 0;
}