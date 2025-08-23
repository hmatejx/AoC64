#ifndef __STACK_H__
#define __STACK_H__

#include <stdint.h>
#include <assert.h>


// The stack structure
template <typename T, typename IType, uint16_t MAX_SIZE>
struct Stack {
    // Data
	T arr[MAX_SIZE];
    IType sp = 0;

    // Removes and returns the top element
    T pop() { assert(this->sp > 0); return this->arr[--this->sp]; }
    // Returns the top element without removing it
    T peek() const { return this->arr[this->sp - 1]; }
    // Adds an element to the top
    void push(const T& item) { assert(this->sp < MAX_SIZE); this->arr[this->sp++] = item; }
    // Returns the number of elements
    IType size() const { return this->sp; }
    // Clears the stack
    void clear() { this->sp = 0; }
};

#endif //__STACK_H__
