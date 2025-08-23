#include <string.h>
#include "AoC64.h"
#if __has_include("input.h")
  #include "input.h"          // user’s private input (gitignored)
#else
  #include "input.example.h"  // checked-in sample input
#endif


int8_t most_common_digit(const uint8_t digit, const uint16_t* idx = NULL, const uint16_t size = input_len)
{
    uint16_t res = 0;
    if (idx == NULL) {
        for (uint16_t i = 0; i < size; ++i) {
            res += input[i][digit] == '1' ? 1 : 0;
        }
    } else {
        for (uint16_t i = 0; i < size; ++i) {
            res += input[idx[i]][digit] == '1' ? 1 : 0;
        }
    }
    return 2*res > size ? 1 : (2*res == size ? 0 : -1);
}

uint16_t get_bitmask(const uint16_t len)
{
    return (~(uint16_t(0U))) >> ((sizeof(uint16_t) << 3) - len);
}

uint16_t to_number(const char* number) {
    uint16_t ret = 0;
    const uint8_t n = strlen(number);
    for (uint8_t i = 0; i < n; ++i) {
        ret |= (number[i] == '1') << (n - 1 - i);
    }
    return ret;
}

int main(void)
{
    init(3);
    const uint8_t ndigits = strlen(input[0]);

    // part 1
    uint16_t res = 0;
    for (uint8_t i = 0; i < ndigits; ++i) {
        tick(i & (uint8_t)7);
        res |= (most_common_digit(i) > 0) << (ndigits - 1 - i);
    }
    uint32_t gamma = res;
    uint32_t epsilon = ~gamma & get_bitmask(ndigits);
    printf("part 1: %lu\n", gamma*epsilon);


    // part 2
    uint16_t n_o2 = input_len;
    uint16_t n_co2 = input_len;
    uint16_t idx_o2[input_len];
    uint16_t idx_co2[input_len];
    for (uint16_t i = 0; i < input_len; ++i) {
        idx_o2[i] = i;
        idx_co2[i] = i;
    }
    for (uint8_t d = 0;  d < ndigits; ++d) {
        tick(d & (uint8_t)7);
        if (n_o2 > 1) {
            uint16_t i_o2 = 0;
            const int8_t m = most_common_digit(d, idx_o2, n_o2);
            for (uint16_t i = 0; i < n_o2; ++i) {
                const uint16_t idx = idx_o2[i];
                const char digit = input[idx][d];
                if ((m >= 0 && digit == '1') || (m == -1 && digit == '0')) {
                    idx_o2[i_o2++] = idx;
                }
            }
            n_o2 = i_o2;
        }
        if (n_co2 > 1) {
            uint16_t i_co2 = 0;
            const int8_t m = most_common_digit(d, idx_co2, n_co2);
            for (uint16_t i = 0; i < n_co2; ++i) {
                const uint16_t idx = idx_co2[i];
                const char digit = input[idx][d];
                if ((m >= 0 && digit == '0') || (m == -1 && digit == '1')) {
                    idx_co2[i_co2++] = idx;
                }
            }
            n_co2 = i_co2;
        }
    }

    uint32_t o2 = to_number(input[idx_o2[0]]);
    uint32_t co2 = to_number(input[idx_co2[0]]);
    printf("part 2: %lu\n", o2*co2);

    finish();
    return 0;
}
