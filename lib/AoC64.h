#include <cstdint>
#include <cstdio>
#include <string.h>

typedef _BitInt(24) int24_t;
typedef unsigned _BitInt(24) uint24_t;

#ifdef __mos6502__
#include <c64.h>
#include <peekpoke.h>


constexpr uint16_t a_SCREEN = 0x0400;
constexpr uint16_t a_COLOR = 0xD800;
constexpr uint8_t TICKER_HI = 0x2A;
constexpr uint8_t TICKER_LO = 0x2D;
auto p_SCREEN = reinterpret_cast<uint8_t *>(a_SCREEN);
auto p_COLOR = reinterpret_cast<uint8_t *>(a_COLOR);


#define _KERNAL static __inline__ __attribute__((__always_inline__, __nodebug__))
#define _ASM __attribute__((leaf)) __asm__

// Waits until keybord hit
_KERNAL void _WAIT_KBHIT() { _ASM ("loop%=: JSR $FFE4\nBEQ loop%=":::"a","x","y"); }

// Clears the screen (resets both character and color data) and moves cursor to top-left (0,0)
_KERNAL void _CLRSCR() { _ASM volatile("JSR $E544":::"a","x","y","c"); }

// Set current text cursor position
_KERNAL void _SET_CURSOR(uint8_t y, uint8_t x) {_ASM volatile("CLC\nJSR $FFF0"::"x"(y),"y"(x):"a","c"); }

// returns the JiffyClock (number of jiffies elapsed since boot, 50/s on a PAL and 60/s on a NTSC C64)
static uint24_t _start_time, _end_time;
_KERNAL uint24_t get_jiffies() {
    return ((uint24_t)(*(volatile uint8_t*)0x00A0) << 16) |
           ((uint24_t)(*(volatile uint8_t*)0x00A1) <<  8) |
            (uint24_t)(*(volatile uint8_t*)0x00A2);
}

// calculates the time difference up to 1/100s of a second (precision ~ 20ms for PAL, 17ms for NTSC)
static inline char* ELAPSED() {
    static char buffer[10];
    memset(buffer, 0, 10);
    const uint32_t elapsed = (_end_time - _start_time) << 1; // time / 50 * 100
    sprintf(buffer, "%lu.%d", elapsed/100, (uint8_t)elapsed % 100);
    return buffer;
}

// blits a rectangular array of petscii bytes on screen (no bounds checking)
static inline void blit_petscii(const uint8_t row, const uint8_t col,
                                const uint8_t dimy, const uint8_t dimx,
                                const uint8_t* const screencode, const uint8_t* const colorcode) {
    for (uint8_t i = 0; i < dimy; ++i) {
        const uint16_t off = 40*(row + i) + col;
        const uint8_t stride = dimx*i;
        for (uint8_t j = 0; j < dimx; ++j) {
            const uint8_t idx = stride + j;
            if (screencode[idx] != 32)
                POKE(a_SCREEN + off + j, screencode[idx]);
                POKE(a_COLOR + off + j, colorcode[idx]);
        }
    }
}


// updates the ticker by one tick to state j
static inline void tick(const uint8_t j) {
    static uint8_t state = 7;
    if (j == state)
        return;
    constexpr uint16_t offset = a_SCREEN + 71;
    if (j == 0) {
        POKE(offset+state, TICKER_LO);
        POKE(offset, TICKER_HI);
    } else {
        uint16_t addr = a_SCREEN + 70 + j;
        POKE(addr++, TICKER_LO);
        POKE(addr, TICKER_HI);
    }
    state = j;
}


// initialize the custom screen
#define _FG_COLOR 5
#define _BG_COLOR 0
void init(const uint8_t day) {
    //select unshifted mode
    POKE(0xD018, 21);

    // set colors and clear screen
    _CLRSCR();
    VIC.bgcolor0 = _BG_COLOR;
    VIC.bordercolor = _BG_COLOR;
    memset(p_COLOR, _FG_COLOR, 1000);
    POKE(646, _FG_COLOR);

    // banner
    memset(p_SCREEN, 64, 40);
    memset(p_SCREEN + 80, 64, 40);
    memset(p_SCREEN + 70, TICKER_LO, 10);
    POKE(a_SCREEN + 109, 113);
    POKE(a_SCREEN + 29, 114);
    POKE(a_SCREEN + 69, 93);
    memset(p_COLOR + 920, 1, 40);
    memset(p_SCREEN + 920, 111, 40);
    memset(p_SCREEN + 920 + 40, 119, 40);

    _SET_CURSOR(1, 0);
    printf("advent of code 2021 / day ");
    if (day < 10)
        printf("0");
    printf("%d", day);
    _SET_CURSOR(22, 0);
    printf("[2021] ");
    POKE(646, 7);
    printf("%u*", 2*day);
    POKE(646, _FG_COLOR);
    _SET_CURSOR(4, 0);

    // christmass drawing

    const uint8_t code[] = {32, 32, 32, 42, 32, 32, 32,
                            32, 32, 233,167,223,32, 32,
                            32, 32, 233,167,81, 32, 32,
                            32, 233, 81,167,167,223,32,
                            32, 233,167,167,81, 223,32,
                            233,81, 167,167,167,167,223,
                            32, 32, 32, 160,32, 32, 32,
                            111,111,96, 160,96, 111,111};
    const uint8_t colr[] = {255,255,255,7,  255,255,255,
                            255,255,5,  5,  5,  255,255,
                            255,255,5,  5,  14, 255,255,
                            255,5,  10, 5,  5,  5,  255,
                            255,5,  5,  5,  7,  5,  255,
                            5,  3,  5,  5,  5,  5,  5,
                            255,255,255,8,  255,255,255,
                            1,  1,  0,  8,  0,  1,  1  };
    blit_petscii(16, 32, 8, 7, code, colr);

    _start_time = get_jiffies();
}


// final state
void finish() {
    _end_time = get_jiffies();

    printf("\nrun time: %s seconds", ELAPSED());

    /*_SET_CURSOR(12, 13);
    printf("press any key\n");
    _WAIT_KBHIT(); */
}

#else
void init(const uint8_t day) {};
void tick(const uint8_t j) {};
void finish() {};
#endif