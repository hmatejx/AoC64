#include <stdio.h>
#include "AoC64.h"
#if __has_include("input.h")
  #include "input.h"          // user’s private input (gitignored)
#else
  #include "input.example.h"  // checked-in sample input
#endif

// based on https://github.com/FransFaase/AdventOfCode2021/blob/main/src/day18_4sol.cpp
constexpr int8_t MAX_TREE_SIZE = 32;
struct Tree {
    int16_t v[MAX_TREE_SIZE];
    Tree() { clear(); }
    void clear() { for (int8_t i = 1; i < MAX_TREE_SIZE; i++) v[i] = -1; }
};

void parse(const char * &s, int16_t *v, int8_t d) {
    if (*s == '[') {
        s++;
        parse(s, v - d, d/2);
        s++;
        parse(s, v + d, d/2);
        s++;
    } else
        *v = *s++ - '0';
}

void add(Tree &l, Tree &r, Tree &sum) {
    sum.clear();
    // temporarily use a 2x larger tree
    int16_t isum[2*MAX_TREE_SIZE];
    isum[MAX_TREE_SIZE] = -1;
    for (int8_t i = 1; i < MAX_TREE_SIZE; ++i) {
        isum[i] = l.v[i];
        isum[MAX_TREE_SIZE + i] = r.v[i];
    }
    // make a combined tree
    for (int8_t i = 0; i < 2*MAX_TREE_SIZE; ++i) {
        if ((i & 3) == 1 && isum[i] != -1) {
            for (int8_t j = i - 1; j > 0; --j)
                if (isum[j] != -1) {
                    isum[j] += isum[i];
                    break;
                }
            isum[i++] = -1;
            isum[i++] = 0;
            for (int8_t j = i + 1; j < 2*MAX_TREE_SIZE; j++)
                if (isum[j] != -1) {
                    isum[j] += isum[i];
                    break;
                }
            isum[i] = -1;
        }
    }
    // shrink it
    for (int8_t i = 1; i < MAX_TREE_SIZE; i++) {
        sum.v[i] = isum[2*i];
    }
    for (int8_t i = 1; i < MAX_TREE_SIZE;) {
        if (sum.v[i] < 10)
            i++;
        else {
            int16_t n1 = sum.v[i] / 2;
            int16_t n2 = sum.v[i] - n1;
            if ((i & 1) == 1) {
                int8_t p_i;
                for (p_i = i - 1; p_i > 0; --p_i)
                    if (sum.v[p_i] != -1) {
                        sum.v[p_i] += n1;
                        break;
                    }
                int8_t n_i;
                for (n_i = i + 1; n_i < MAX_TREE_SIZE; ++n_i)
                    if (sum.v[n_i] != -1) {
                        sum.v[n_i] += n2;
                        break;
                    }
                sum.v[i] = 0;
                i = p_i > 0 ? p_i : n_i;
            } else {
                sum.v[i] = -1;
                int8_t o;
                if ((i & 3) == 0)       o = 2;
                else if ((i & 7) == 0)  o = 4;
                else if ((i & 15) == 0) o = 8;
                else                    o = 1;
                sum.v[i] = -1;
                sum.v[i - o] = n1;
                sum.v[i + o] = n2;
                i -= o;
            }
        }
    }
}

inline int16_t magnitude(int16_t *v, int8_t d) {
    return *v == -1 ? 3 * magnitude(v - d, d/2) + 2 * magnitude(v + d, d/2) : *v;
}

int main(void) {
    init(18);

    Tree numbers[n_input], result;
    parse(input[0], numbers[0].v + MAX_TREE_SIZE/2, 8);
    Tree sum = numbers[0];
    for (int8_t i = 1; i < n_input; ++i) {
        tick(i & (uint8_t)7);
        parse(input[i], numbers[i].v + MAX_TREE_SIZE/2, 8);
        add(sum, numbers[i], result);
        sum = result;
    }
    printf("part 1: %d\n", magnitude(sum.v + MAX_TREE_SIZE/2, 8));

    int16_t largest = 0;
    int16_t t = 0;
    for (int8_t i = 0; i < n_input; ++i) {
        for (int8_t j = 0; j < n_input; ++j) {
            tick((++t >> 3) & (uint8_t)7);
            add(numbers[i], numbers[j], result);
            int16_t m = magnitude(result.v + MAX_TREE_SIZE/2, 8);
            if (m > largest)
                largest = m;
        }
    }
    printf("part 2: %d\n", largest);

    finish();
}