#ifndef __STACK_REU_H__
#define __STACK_REU_H__

#include <stdint.h>
#include <assert.h>
#include "reucpy.h"


// The stack structure. Before use call the Init(base_addr) method
template <typename T, typename IType, uint16_t MAX_SIZE>
struct StackREU {
    // Data
    T record;
    IType sp = 0;
    reu_addr_t base_addr;
    reu_addr_t end_addr;
    static const uint16_t record_size = (uint16_t)sizeof(T);
    static const reu_addr_t memory_size = (reu_addr_t)MAX_SIZE*record_size;

    // Initializes the stack on the REU, needs to be called before first use
    void init(reu_addr_t addr) { this->base_addr = addr; this->end_addr = addr + memory_size; this->sp = 0; };
    // Removes and returns the top element (2 copy operations, 1 from REU to RAM, 1 from RAM to RAM)
    T pop();
    // Removes and returns the top element (1 copy operation from REU to RAM location given by the item)
    void popInto(T* const item);
    // Adds an element to the top
    void push(const T& item);
    // Gets i-th element (2 copy operations, 1 from REU to RAM, 1 from RAM to RAM)
    T get(const IType i);
    // Gets i-th element (1 copy operations from REU to RAM given by the item)
    void getInto(const IType i, T* const item) const;
    // Returns the number of elements
    IType size() const { return this->sp; }
    // Clears the stack (only resets the stack pointer, memory is not actually erased)
    void clear() { this->sp = 0; }

    // Internal, do not use
    reu_addr_t _get_addr(const IType i) const { assert(i < MAX_SIZE); return this->base_addr + (reu_addr_t)this->record_size*i; }
};


// implementation
template <typename T, typename IType, uint16_t MAX_SIZE>
T StackREU<T, IType, MAX_SIZE>::pop() {
    reucpy((void *)&this->record, _get_addr(--this->sp), sizeof(T), REU2RAM);
	return this->record;
}


template <typename T, typename IType, uint16_t MAX_SIZE>
void StackREU<T, IType, MAX_SIZE>::popInto(T* const item) {
    reucpy((void *)item, _get_addr(--this->sp), sizeof(T), REU2RAM);
}


template <typename T, typename IType, uint16_t MAX_SIZE>
T StackREU<T, IType, MAX_SIZE>::get(const IType i) {
    reucpy((void *)&this->record, _get_addr(i), sizeof(T), REU2RAM);
	return this->record;
}


template <typename T, typename IType, uint16_t MAX_SIZE>
void StackREU<T, IType, MAX_SIZE>::getInto(const IType i, T* item) const {
    reucpy((void *)item, _get_addr(i), sizeof(T), REU2RAM);
}


template <typename T, typename IType, uint16_t MAX_SIZE>
void StackREU<T, IType, MAX_SIZE>::push(const T& item) {
    reucpy((void *)&item, _get_addr(this->sp++), sizeof(T), RAM2REU);
}

#endif //__STACK_REU_H__
