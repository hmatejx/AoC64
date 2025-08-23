#include <stdint.h>
#include <assert.h>
#include <string.h>
#include "AoC64.h"
#include "reucpy.h"
#if __has_include("input.h")
  #include "input.h"          // user’s private input (gitignored)
#else
  #include "input.example.h"  // checked-in sample input
#endif


constexpr uint16_t BUFF_SIZE = (uint16_t)(input_dim + 2*50)*(input_dim + 2*50) / 8 + 1;
constexpr uint16_t MAX_DIM = input_dim + 2*50; // space for only up to 50 enhancements

struct Image {
    // backend arrays
    uint8_t buff1[BUFF_SIZE] = {0};
    uint8_t buff2[BUFF_SIZE] = {0};
    // for double buffering :)
    uint8_t* original = &this->buff1[0];
    uint8_t* enhanced = &this->buff2[0];
    // image parameters
    uint8_t dim_original = 0, dim_enhanced = 0, bg_original = 0, bg_enhanced = 0;
    // helps performance to hardcode bit masks
    static constexpr uint8_t bits[8] = {1, 2, 4, 8, 16, 32, 64, 128};

    // initizalize from input
    void init();
    // flips buffer pointers, backgrounds, and clears the enhanced buffer
    void flip_and_clear();
    // always writes to enhanced
    void setbit(const uint8_t i, const uint8_t j);
    // always reads from original
    uint8_t getbit(const uint8_t i, const uint8_t j) const;
    // calculate index from a 3x3 region centered on pixel i,j
    uint16_t getindex(uint8_t i, uint8_t j) const;
    // enhancement algorithm
    void enhance(const uint8_t nsteps, const uint8_t* const algo);
    // count the set pixels
    uint16_t count() const;
};

inline void Image::flip_and_clear() {
    uint8_t* tmp1 = this->original; this->original = this->enhanced; this->enhanced = tmp1;
    uint8_t tmp2 = this->bg_original; this->bg_original = this->bg_enhanced; this->bg_enhanced = tmp2;
    uint8_t tmp3 = this->dim_original; this->dim_original = this->dim_enhanced; this->dim_enhanced = tmp3;
    memset_reu(0, (uintptr_t)this->enhanced, BUFF_SIZE);
}

void Image::init() {
    reuset(0, 0, 1);
    this->dim_original = input_dim;
    this->dim_enhanced = input_dim + 2;
    this->bg_original = 0;
    this->bg_enhanced = 0;
    for (uint8_t j = 0; j < input_dim; ++j)
        for (uint8_t i = 0; i < input_dim; ++i)
            if (input_map[j][i] == '#')
                this->setbit(i, j);
    uint8_t* tmp = this->original;
    this->original = this->enhanced;
    this->enhanced = tmp;
}

inline void Image::setbit(const uint8_t i, const uint8_t j) {
    const uint16_t bit_idx = MAX_DIM*j + i;
    this->enhanced[bit_idx >> 3] |= this->bits[bit_idx & (uint8_t)7];
}

inline uint8_t Image::getbit(const uint8_t i, const uint8_t j) const {
    // note: below check handles also negative indices wrapped aroud to 255
    if (i > this->dim_original || j >= this->dim_original)
        return this->bg_original;
    const uint16_t bit_idx = MAX_DIM*j + i;
    if (this->original[bit_idx >> 3] & bits[bit_idx & (uint8_t)7])
        return 1 - this->bg_original;
    return this->bg_original;
}

inline uint16_t Image::getindex(uint8_t i, uint8_t j) const {
    uint16_t idx = 0;
    const uint8_t jm1 = j - 1, jp1 = j + 1, im1 = i - 1, ip1 = i + 1;
    idx |= this->getbit(im1, jm1); idx <<= 1;
    idx |= this->getbit(i  , jm1); idx <<= 1;
    idx |= this->getbit(ip1, jm1); idx <<= 1;
    idx |= this->getbit(im1, j  ); idx <<= 1;
    idx |= this->getbit(i  , j  ); idx <<= 1;
    idx |= this->getbit(ip1, j  ); idx <<= 1;
    idx |= this->getbit(im1, jp1); idx <<= 1;
    idx |= this->getbit(i  , jp1); idx <<= 1;
    idx |= this->getbit(ip1, jp1);
    return idx;
}

static uint16_t t = 0;
void Image::enhance(const uint8_t nsteps, const uint8_t* const algo) {
    uint8_t step = 0;
    while (step < nsteps) {
        step += 1;
        // enhance and switch background if necessary
        this->dim_enhanced = this->dim_original + 2;
        this->bg_enhanced = algo[0] ? step & (uint8_t)1 : 0;
        for (uint8_t j = 0; j < this->dim_enhanced; ++j) {
            tick(t++ >> 1 & (uint8_t)7);
            const uint8_t jj = j-1;
            for (uint8_t i = 0; i < this->dim_enhanced; ++i) {
                if (algo[this->getindex(i-1, jj)] != this->bg_enhanced)
                    this->setbit(i, j);
            }
        }
        this->flip_and_clear();
    }
}

inline uint16_t Image::count() const {
    uint16_t c = 0;
    for (uint8_t j = 0; j < this->dim_original; ++j)
        for (uint8_t i = 0; i < this->dim_original; ++i)
            c += this->getbit(i, j);
    return c;
}


int main(void) {
    init(20);

    // transform the algo input
    uint8_t algo[512] = {0};
    for (int16_t i = 0; i < 512; ++i)
        if (input_algo[i] == '#')
            algo[i] = 1;

    // load the input image
    Image image;
    image.init();

    // enhance
    image.enhance(2, &algo[0]);
    printf("part 1: %u\n", image.count());
    image.enhance(48, &algo[0]);
    printf("part 2: %u\n", image.count());

    finish();
}