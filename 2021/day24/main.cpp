#include <stdint.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "AoC64.h"
#include "stack.h"
#if __has_include("input.h")
  #include "input.h"          // user’s private input (gitignored)
#else
  #include "input.example.h"  // checked-in sample input
#endif


struct Digit {
    uint8_t depth;
    uint8_t digit;
};


// modifies the version number in-place
char* solve(char* version) {
    Stack<Digit, uint8_t, 128> stack;
    for (uint8_t i = 0; i < 14; ++i) {
        // parse the parameters from input
        short n1, n2, n3;
        uint8_t ii = 18*i + 4;
        sscanf(program[ii], "%*s %*s %hd", &n1); ii += 1;
        sscanf(program[ii], "%*s %*s %hd", &n2); ii += 10;
        sscanf(program[ii], "%*s %*s %hd", &n3);
        // push 26-ary digit to stack
        if (n1 == 1) {
            stack.push(Digit {i, (uint8_t)n3});
        // pop 26-ary digit from stack
        } else if (n1 == 26) {
            Digit z = stack.pop();
            version[i] = version[z.depth] + z.digit + (int8_t)n2;
            if (version[i] > '9') {
                version[z.depth] -= (version[i] - '0') - 9;
                version[i] = '9';
            }
            if (version[i] < '1') {
                version[z.depth] += 1 - (version[i] - '0');
                version[i] = '1';
            }
        }
    }
    return version;
}


int main(void) {
    init(24);

    char max[15] = "99999999999999";
    char min[15] = "11111111111111";

    printf("part 1: %s\n", solve(max));
    printf("part 2: %s\n", solve(min));

    finish();
}