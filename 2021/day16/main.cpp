#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AoC64.h"
#if __has_include("input.h")
  #include "input.h"          // user’s private input (gitignored)
#else
  #include "input.example.h"  // checked-in sample input
#endif

inline uint8_t to_nibble(const char c) {
    if (c >= '0' && c <= '9')
        return c - '0';
    return c - 'A' + 10;
}

uint16_t get_bits_increment_offset(uint8_t nbits, const char* packet, uint16_t& bit_offset) {
    uint16_t res = 0;
    for (int8_t bit = 0; bit < nbits; ++bit) {
        const uint16_t offset = bit_offset + bit;
        if ((to_nibble(packet[offset >> 2]) >> (3 - (offset & 3))) & 1)
            res |= 1 << (nbits - 1 - bit);
    }
    bit_offset += nbits;
    return res;
}
#define get_bits(nbits) get_bits_increment_offset((nbits), packet, bit_offset)


// global variable hack for part 1
uint16_t g_sum_versions = 0;

// returns the number of bits in the packet
uint64_t decode(const char* packet, uint16_t& bit_offset, uint16_t& bits_read) {

    uint16_t start_offset = bit_offset;
    uint64_t value;

    // get version and type
    uint8_t version = get_bits(3);
    uint8_t type = get_bits(3);
    g_sum_versions += version;

    // consider type
    switch (type) {
        // literal value packet
        case 4:
            {
                value = 0;
                //uint8_t n_nibbles = 0, more;
                for (;;) {
                    uint8_t more = get_bits(1);
                    value |= get_bits(4);
                    // nibble padded with bit 0, last nibble
                    if (!more)
                        break;
                    // make place for next nibble
                    value <<= 4;
                    //++n_nibbles;
                }
            }
            break;

        // operator packet
        case 0:
        case 1:
        case 2:
        case 3:
            {
                // get the total total bit length or total number of sub-packets
                uint16_t subpackets_len, num_subpackets;
                if (get_bits(1) == 0) {
                    subpackets_len = get_bits(15);
                    num_subpackets = 0xFFFF;
                } else {
                    subpackets_len = 0xFFFF;
                    num_subpackets = get_bits(11);
                }
                // process the subpackets
                uint16_t tot_len = 0, len;
                uint8_t num = 0;
                if (type == 0 || type == 3)
                    value = 0;
                else if (type == 2)
                    value = 0xFFFFFFFFFFFFFFFF;
                else
                    value = 1;
                while (tot_len < subpackets_len && num < num_subpackets) {
                    uint64_t v = decode(packet, bit_offset, len);
                    switch (type) {
                        case 0: value += v; break;
                        case 1: value *= v; break;
                        case 2: if (v < value) value = v; break;
                        case 3: if (v > value) value = v; break;
                    }
                    tot_len += len;
                    num++;
                }
            }
            break;
        default:
            uint16_t len;
            get_bits(get_bits(1) == 0 ? 15 : 11);
            uint64_t v1 = decode(packet, bit_offset, len);
            uint64_t v2 = decode(packet, bit_offset, len);
            if (type == 5)
                value = v1 > v2 ? 1 : 0;
            else if (type == 6)
                value = v1 < v2 ? 1 : 0;
            else
                value = v1 == v2 ? 1 : 0;
    }
    // update the number of bits read (bit_offset is updated automatically in get_bits)
    bits_read = bit_offset - start_offset;
    tick((bit_offset >> 8) & (uint8_t)7);

    return value;
}


int main(void) {
    init(16);

    uint16_t bit_offset = 0, len;
    uint64_t value = decode(input[0], bit_offset, len);

    printf("part 1: %u\n", g_sum_versions);
    printf("part 2: %llu\n", value);

    finish();
}