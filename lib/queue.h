#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdint.h>
#include <assert.h>

// The queue structure
template <typename T, typename IType, uint16_t MaxSize>
struct Queue {
    // data
    T arr[MaxSize];
    IType front = 0;
    IType back = 0;
    IType count = 0;

    // Removes and returns the front element
    T pop();
    // Removes and returns the back element
    T popBack();
    // Adds an element to the back
    void push(const T& item);
    // Returns the front element without removing it
    T peek() const { return this->arr[this->front]; }
    // Returns the back element without removing it
    T peekBack() const { return this->arr[this->back]; }
    // Returns the number of elements
    IType size() const { return this->count; }
    // Clears the queue
    void clear() { this->front = 0; this->back = 0; count = 0; }
};


template <typename T, typename IType, uint16_t MaxSize>
T Queue<T, IType, MaxSize>::pop() {
    assert(this->count > 0);

    const T item = this->arr[this->front];
    if (++this->front == MaxSize)
        this->front = 0;
    this->count--;
    return item;
}

template <typename T, typename IType, uint16_t MaxSize>
void Queue<T, IType, MaxSize>::push(const T& item) {
    assert(this->count < MaxSize);

    this->arr[this->back] = item;
    if (++this->back == MaxSize)
        this->back = 0;
    this->count++;
}

template <typename T, typename IType, uint16_t MaxSize>
T Queue<T, IType, MaxSize>::popBack() {
    assert(this->count > 0);

    if (this->back == 0)
        this->back = MaxSize - 1;
    else
        this->back--;
    const T item = this->arr[this->back];
    this->count--;
    return item;
}

#endif //__QUEUE_H__
