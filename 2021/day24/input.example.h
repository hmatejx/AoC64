#include <stdint.h>

const char* const program[] = {
"inp w",
"add z w",
"mod z 2",
"div w 2",
"add y w",
"mod y 2",
"div w 2",
"add x w",
"mod x 2",
"div w 2",
"mod w 2"
};

constexpr uint8_t n_lines = sizeof(program) / sizeof(program[0]);
