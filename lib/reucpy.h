#ifndef __REUCPY_H__
#define __REUCPY_H__

#include <stdint.h>
#include <string.h>
#include <assert.h>

#define RAM2REU 0x90 // Copy from C64 RAM to REU memory
#define REU2RAM 0x91 // Copy from REU memory to C64 RAM
#define REUSWAP 0x92 // Swap contents between REU and C64 RAM addresses
#define REUCMP  0x93 // Compare contents between REU and C64 RAM

// unsigned 24 bit integer type
typedef unsigned _BitInt(24) reu_addr_t;

static volatile uint8_t* const reu_command = (uint8_t*)0xDF01;
static volatile uintptr_t* const reu_c64_addr = (uintptr_t*)0xDF02;
static volatile reu_addr_t* const reu_cart_addr = (reu_addr_t*)0xDF04;
static volatile uint16_t* const reu_xfer_length = (uint16_t*)0xDF07;
static volatile uint8_t* const reu_address_ctl = (uint8_t*)0xDF0A;

// Like memcpy, but copies data between C64 RAM and REU.
// size: number of bytes to write. Note that 0 is interpreted as 65536 bytes!
// direction: specifies the direction of copy. Use one of the defines above.
void reucpy(void* c64_addr, reu_addr_t reu_addr, uint16_t size, uint8_t direction);

// Like memset, for setting data in REU memory.
// reu_addr: starting byte address in REU memory.
// val: the byte value to fill.
// size: the number of bytes to fill. Note that 0 is interpreted as 65536 bytes!
void reuset(reu_addr_t reu_addr, uint8_t val, uint16_t size);

// Like memset, for setting data in C64 memory.
// reu_addr: byte address in REU memory holding the value.
// addr: C64 address to fill.
// size: the number of bytes to fill. Note that 0 is interpreted as 65536 bytes!
void memset_reu(reu_addr_t reu_addr, uintptr_t addr, uint16_t size);

// Like memcpy, for copying data from src to dest (can overlap) by using the REU.
// Uses (clobbers) up to 256 initial bytes on the REU "zero page"
// dest: C64 destination address.
// src: C64 source address
// count: the number of bytes to copy.
void memcpy_reu(void* dest, const void* src, uint16_t count);

// Initializes REU, and returns the number of 64KB REU banks that were detected:
// 0 - no REU detected.
// 1 - 64KB REU detected,
// 256: 16MB REU detected.
// Call this once before calling reucpy().
const uint16_t reu_init(void);

// Clear REU banks 0 .. nbanks-1.
// nbanks: number of banks to clear.
void reu_clear(uint16_t nbanks);

// Returns the number of banks needed to hold data of 'size' bytes.
// size: the size of the data in bytes.
const uint16_t banks_needed(uint32_t size);


void reucpy(void* c64_addr, reu_addr_t reu_addr, uint16_t size, uint8_t direction)
{
  // Call reu_init() first if this assert() triggers, or if manually
  // having done REU accesses in between, be sure to reset bits 7 and 6
  // of REU address control register back to zero.
  assert((*reu_address_ctl & 0xC0) == 0);

  *reu_c64_addr = (uintptr_t)c64_addr;
  *reu_cart_addr = reu_addr;
  *reu_xfer_length = size;
  *reu_command = direction;
}

void reuset(reu_addr_t reu_addr, uint8_t val, uint16_t size)
{
  *reu_address_ctl = (*reu_address_ctl & 0x3F) | 0x80; // Fix C64 address, advance REU address

  *reu_c64_addr = (uintptr_t)&val;
  *reu_cart_addr = reu_addr;
  *reu_xfer_length = size;
  *reu_command = RAM2REU;

  *reu_address_ctl = *reu_address_ctl & 0x3F; // Restore both C64 and REU addresses to advance per each transferred byte
}

void memset_reu(reu_addr_t reu_addr, uintptr_t addr, uint16_t size)
{
  *reu_address_ctl = (*reu_address_ctl & 0x3F) | 0x40; // Fix the REU address, advance C64 address

  *reu_c64_addr = addr;
  *reu_cart_addr = reu_addr;
  *reu_xfer_length = size;
  *reu_command = REU2RAM;

  *reu_address_ctl = *reu_address_ctl & 0x3F; // Restore both C64 and REU addresses to advance per each transferred byte
}

void memcpy_reu(void* dest, const void* src, uint16_t count) {
  reucpy((void *)src, 0, count, RAM2REU);
  reucpy(dest, 0, count, REU2RAM);
}

const uint16_t reu_init()
{
  *reu_address_ctl = *reu_address_ctl & 0x3F; // Make both C64 and REU addresses advance per each transferred byte

  char data[5] = { 'a', 'r', 'e', 'u', 0 };
  for(int16_t bank = 255; bank >= 0; --bank)
  {
    data[0] = (uint8_t)bank;
    reucpy(data, (reu_addr_t)bank << 16, 4, RAM2REU);
  }
  memset(data, 0, sizeof(data));
  for(int bank = 0; bank < 256; ++bank)
  {
    reucpy(data, (reu_addr_t)bank << 16, 4, REU2RAM);
    if (data[0] != bank || data[1] != 'r' || data[2] != 'e' || data[3] != 'u')
      return bank;
  }

  return 256; // Full 16MB of REU available
}

void reu_clear(uint16_t nbanks) {
    for (uint16_t bank = 0; bank < nbanks; ++bank) {
        reuset((reu_addr_t)bank << 16, 0, 0);
    }
}

const uint16_t banks_needed(uint32_t size) {
    return (size >> 16) + 1;
}

#endif // __REUCPY_H__