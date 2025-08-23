#include <cstdio>
#include <string.h>
#include "AoC64.h"
#include "reucpy.h"
#if __has_include("input.h")
  #include "input.h"          // user’s private input (gitignored)
#else
  #include "input.example.h"  // checked-in sample input
#endif

inline int16_t min(const int16_t a, const int16_t b) { return a > b ? b : a; }
inline int16_t max(const int16_t a, const int16_t b) { return a < b ? b : a; }
#define SWAP(a, b) do { int16_t tmp = (a); (a) = (b); (b) = tmp; } while(0)

void orient(void) {
   for (int16_t i = 0; i < n_lines; ++i) {
        int16_t* const l = lines[i];
        if (l[0] > l[2]) {
            SWAP(l[0], l[2]);
            if (l[1] != l[3]) {
                SWAP(l[1], l[3]);
            }
        } else if (l[0] == l[2] && l[1] > l[3]) {
            SWAP(l[1], l[3]);
        }
    }
}

int main(void) {
    init(5);
    reuset(0, 0, 1);

    // first, orient the lines canonically
    orient();

    // process sectors of size 64x64
    int16_t res = 0, res2 = 0;
    constexpr int16_t range = 1024;
    constexpr int8_t stride = 64;
    uint8_t S1[stride][stride] = {{}}; // 4KB
    uint8_t S2[stride][stride] = {{}}; // 4KB
    uint8_t i = 0, j = 0;

    uint8_t t = 0;
    for (int16_t y = 0; y < range; y += stride, ++i) {
        const int16_t yl = y, yu = y + stride - 1;

        for (int16_t x = 0; x < range; x += stride, ++j) {
            const int16_t xl = x, xu = x + stride - 1;

            tick(t & (uint8_t)7); ++t;

            // reset sector bitmaps
            constexpr uint16_t size = stride*stride;
            memset_reu(0, (uintptr_t)S1, size);
            memset_reu(0, (uintptr_t)S2, size);

            // find lines that intersect the sector
            for (int16_t k = 0; k < n_lines; ++k) {
                const int16_t x1 = lines[k][0], y1 = lines[k][1], x2 = lines[k][2], y2 = lines[k][3];

                // line is horizontal [>]
                if (y1 == y2) {
                    if (y1 > yu || y1 < yl || x2 < xl || x1 > xu)
                        continue;
                    const int16_t ul = max(xl, x1) - xl, uh = min(xu, x2) - xl;
                    const uint8_t ii = y1 - yl;
                    for (uint8_t u = ul; u <= uh; ++u) {
                        ++S1[ii][u];
                        ++S2[ii][u];
                    }

                // line is vertical [^]
                } else if (x1 == x2) {
                    if (x1 < xl || x1 > xu || y2 < yl || y1 > yu)
                        continue;
                    const int16_t vl = max(yl, y1) - yl, vh = min(yu, y2) - yl;
                    const uint8_t jj = x1 - xl;
                    for (uint8_t v = vl; v <= vh; ++v) {
                        ++S1[v][jj];
                        ++S2[v][jj];
                    }

                } else {
                    if ((x1 < xl && x2 < xl) || (y1 < yl && y2 < yl) || (x1 > xu && x2 > xu) || (y1 > yu && y2 > yu))
                        continue;

                    // line is diagonal [/]
                    if (y1 < y2) {
                        if (y1 + (xl - x1) > yu || y1 + (xu - x1) < yl)
                            continue;
                        for (int16_t v = y1 - yl, u = x1 - xl; v <= y2 - yl; ++u, ++v) {
                            if (u > stride || v > stride)
                                break;
                            if (u >= 0 && v >= 0 && u < stride && v < stride)
                                ++S2[v][u];
                        }

                    // line is diagonal [\]
                    }  else {
                        if (y1 - (xl - x1) < yl || y1 - (xu - x1) > yu)
                            continue;
                        for (int16_t v = y1 - yl, u = x1 - xl; v >= y2 - yl; ++u, --v) {
                            if (u > stride || v < 0)
                                break;
                            if (u >= 0 && v >= 0 && u < stride && v < stride)
                                ++S2[v][u];
                        }
                    }
                }
            }
            for (uint8_t u = 0; u < stride; ++u) {
                for (uint8_t v = 0; v < stride; ++ v) {
                    if (S1[u][v] > 1)
                        ++res;
                    if (S2[u][v] > 1)
                        ++res2;
                }
            }
        }
    }

    printf("part 1: %u\n", res);
    printf("part 2: %u\n", res2);

    finish();
}