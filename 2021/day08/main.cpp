#include <stdio.h>
#include <string.h>
#include "AoC64.h"
#if __has_include("input.h")
  #include "input.h"          // user’s private input (gitignored)
#else
  #include "input.example.h"  // checked-in sample input
#endif

inline int8_t is_simple(const uint8_t len) {
    return len == 2 || len == 3 || len == 4 || len == 7;
}

inline uint8_t intersect_size(const char* const a, const uint8_t len_a,
                              const char* const b, const uint8_t len_b) {
    uint8_t ret = 0;
    const char* pb = b;
    for (const char* pa = a; pa < a + len_a && ret < len_b; ++pa)
        for (const char* pb = b; pb < b + len_b; ++pb)
            if (*pa == *pb) {
                ++ret;
                break;
            }
    return ret;
}

uint16_t decode(const char* line) {
    static const uint8_t dlen[] = {6, 2, 5, 5, 4, 5, 6, 3, 7, 6};
    static const char* dptr[10];
    static const char* len5[3];
    static const char* len6[3];
    uint8_t n5 = 0, n6 = 0;
    static char L[2];
    do {
        const char* p = strchr(line, ' ');
        switch (p - line) {
            case 2: dptr[1]    = line; break;
            case 3: dptr[7]    = line; break;
            case 4: dptr[4]    = line; break;
            case 7: dptr[8]    = line; break;
            case 5: len5[n5++] = line; break;
            case 6: len6[n6++] = line;
        }
        line = p + 1;
    } while (*line != '|');
    // find the L in 4
    uint8_t l = 0;
    for (uint8_t i = 0; i < 4; ++i) {
        const char c = dptr[4][i];
        if (c != dptr[1][0] && c != dptr[1][1])
            L[l++] = c;
    }
    // find 3, 5, 2
    for (uint8_t i = 0; i < 3; ++i) {
        if (intersect_size(len5[i], 5, dptr[1], 2) == 2) {  // found 3
            dptr[3] = len5[i];
        } else if (intersect_size(len5[i], 5, L, 2) == 2) { // found 5
            dptr[5] = len5[i];
        } else {
            dptr[2] = len5[i];
        }
    }
    // find 0, 6, 9
    for (uint8_t i = 0; i < 3; ++i) {
        if (intersect_size(len6[i], 6, dptr[4], 4) == 4) {  // found 9
            dptr[9] = len6[i];
        } else if (intersect_size(len6[i], 6, L, 2) == 2) { // found 6
            dptr[6] = len6[i];
        } else {
            dptr[0] = len6[i];
        }
    }
    // decode the digits on the right of '|'
    line += 2;
    uint16_t res = 0, x = 1000;
    const char* pe = strchr(line, 0), *p2;
    do {
        p2 = strchr(line, ' ');
        const uint8_t len = p2 ? p2 - line : pe - line;
        switch (len) {
            case 2: res +=   x; break;
            case 3: res += 7*x; break;
            case 4: res += 4*x; break;
            case 7: res += 8*x; break;
            default:
                for (uint8_t i = 0; i < 10; ++i)
                    if (dlen[i] == len && intersect_size(dptr[i], len, line, len) == len) {
                        res += i*x;
                        break;
                    }
        }
        line = p2 + 1;
        x = x / 10;
    } while (p2);
    return res;
}

int main(void) {
    init(8);

    // part 1
    int16_t res1 = 0;
    for (uint8_t i = 0; i < n_lines; ++i) {
        tick((i >> 3) & 7);
        const char* p1 = strchr(lines[i], '|') + 2, *p2;
        const char* pe = strchr(p1, 0);
        do {
            p2 = strchr(p1, ' ');
            if (is_simple(p2 ? p2 - p1 : pe - p1))
                ++res1;
            p1 = p2 + 1;
        } while (p2);
    }
    printf("part 1: %d\n", res1);

    // part 2
    int32_t res2 = 0;
    for (uint8_t i = 0; i < n_lines; ++i) {
        tick((i >> 2) & 7);
        res2 += decode(lines[i]);
    }
    printf("part 2: %ld\n", res2);

    finish();
}