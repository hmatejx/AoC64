#include <stdio.h>
#include <string.h>
#include "AoC64.h"
#include "reucpy.h"
#if __has_include("input.h")
  #include "input.h"          // user’s private input (gitignored)
#else
  #include "input.example.h"  // checked-in sample input
#endif

constexpr int8_t N_LETTERS = 26;
static uint64_t pfreq[N_LETTERS*N_LETTERS] = {0};
static uint64_t pfreq_new[N_LETTERS*N_LETTERS] = {0};
static uint64_t lfreq[N_LETTERS] = {0};

#define idx(pair) ((*(pair) - 'A')*26 + *((pair) + 1) - 'A')


void polymerize(const int8_t niter) {
    for (int8_t s = 0; s < niter; ++s) {
        memcpy_reu(pfreq_new, pfreq, N_LETTERS*N_LETTERS*sizeof(pfreq[0]));
        for (int8_t i = 0; i < n_rules; ++i) {
            tick((i >> 4) & (uint8_t)7);
            const char* const pair = rules[i][0];
            const int16_t ri = idx(pair);
            const uint64_t pcount = pfreq[ri];
            if (pcount > 0) {
                const char new_element = rules[i][1][0];
                lfreq[new_element - 'A'] += pcount;
                char new_pair[3] = {pair[0], new_element, pair[1]};
                pfreq_new[idx(new_pair)] += pcount;
                pfreq_new[idx(new_pair+1)] += pcount;
                pfreq_new[ri] -= pcount;
            }
        }
        memcpy_reu(pfreq, pfreq_new, N_LETTERS*N_LETTERS*sizeof(pfreq[0]));
    }
}


int main(void) {
    init(14);

    // initialize frequency tables with the initial polymer template
    const int8_t template_len = strlen(polymer_template);
    for (int8_t i = 0; i < template_len; ++i) {
        if (i < template_len - 1)
            pfreq[idx(polymer_template + i)] += 1;
        lfreq[polymer_template[i] - 'A'] += 1;
    }

    // part 1
    polymerize(10);
    uint64_t min = UINT64_MAX, max = 0;
    for (int8_t i = 0; i < N_LETTERS; ++i) {
        if (lfreq[i] == 0) continue;
        if (lfreq[i] < min) min = lfreq[i];
        if (lfreq[i] > max) max = lfreq[i];
    }
    printf("part 1: %lld\n", max - min);

    // part 2
    polymerize(30);
    min = UINT64_MAX, max = 0;
    for (int8_t i = 0; i < N_LETTERS; ++i) {
        if (lfreq[i] == 0) continue;
        if (lfreq[i] < min) min = lfreq[i];
        if (lfreq[i] > max) max = lfreq[i];
    }
    printf("part 2: %lld\n", max - min);

    printf("\n");
    finish();
}