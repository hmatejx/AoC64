#include <stdio.h>
#include <string.h>
#include "AoC64.h"
#include "stack.h"
#include <stdlib.h>
#if __has_include("input.h")
  #include "input.h"          // user’s private input (gitignored)
#else
  #include "input.example.h"  // checked-in sample input
#endif


// global stack
Stack<char, uint8_t, 128> stack;

inline uint8_t matching(const char p1, const char p2) {
    return (p1 == '(' && p2 == ')') || (p1 == '[' && p2 == ']') || (p1 == '{' && p2 == '}') || (p1 == '<' && p2 == '>');
}

int16_t corrupted(const char* line) {
    stack.clear();
    while (*line) {
        if (*line == ')' || *line == ']' || *line == '}' || *line == '>') {
            if (matching(stack.peek(), *line)) {
                stack.pop();
            } else {
                switch (*line) {
                    case ')': return 3;
                    case ']': return 57;
                    case '}': return 1197;
                    case '>': return 25137;
                }
            }
        } else
            stack.push(*line);
        ++line;
    }
    return -stack.size();
}

int64_t completion_score() {
    int64_t score = 0;
    for (int8_t i = stack.size() - 1; i >= 0; --i) {
        score *= 5;
        switch (stack.arr[i]) {
            case '(': score += 1; break;
            case '[': score += 2; break;
            case '{': score += 3; break;
            case '<': score += 4;
        }
    }
    return score;
}

int comp_i64(const void* elem1, const void* elem2)
{
    int64_t f = *((int64_t*)elem1);
    int64_t s = *((int64_t*)elem2);
    if (f > s) return  1;
    if (f < s) return -1;
    return 0;
};

int main(void) {
    init(10);

    int32_t res1 = 0;
    int64_t res2[n_lines] = {0};
    int8_t ncomp = 0;
    for (int8_t i = 0; i < n_lines; ++i) {
        tick((i >> 3) & (uint8_t)7);
        const int16_t score = corrupted(lines[i]);
        if (score > 0) {
            res1 += score;
        } else {
            res2[ncomp++] = completion_score();
        }
    }
    qsort(res2, ncomp, sizeof(*res2), comp_i64);

    printf("part 1: %lu\n", res1);
    printf("part 2: %lld\n", res2[ncomp / 2]);

    finish();
}
