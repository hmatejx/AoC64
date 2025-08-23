#ifndef __MIN_HEAP_REU_H__
#define __MIN_HEAP_REU_H__

#include <stdint.h>
#include <assert.h>
#include "reucpy.h"

// The heap structure implemented on the REU.
// The struct T must provide a priority member variable in the first slot:
// 	struct T __attribute__((packed)) {
//   	int priority; // for example
//	 	...
// 	};
template <typename T, typename IType, uint32_t MAX_SIZE>
struct HeapREU {
	// Data
    reu_addr_t base_addr;
    reu_addr_t end_addr;
	T record;
	IType _size;
	// For memory sizes
	static const uint16_t record_size = (uint16_t)sizeof(T);
    static const reu_addr_t memory_size = (reu_addr_t)(MAX_SIZE*record_size);
	// Initializes the min heap at the REU address addr
	void init(const reu_addr_t addr);
	// Initializes the min heap with initial elements given by items at the REU address addr
	void init(const T* const items, const IType n_items, const reu_addr_t addr);
	// Adds an item to the heap
	void push(const T* const item);
    // Removes and returns the top element (2 copy operations, 1 from REU to RAM, 1 from RAM to RAM)
    T pop();
    // Removes and returns the top element (1 copy operation from REU to RAM location given by the item)
	void popInto(T* const item);
	// Returns the number of elements
	const IType size() const { return this->_size; }

    // Internal helpers, can also be used for performance hacks
    reu_addr_t _get_addr(const IType i) const { return this->base_addr + (reu_addr_t)this->record_size*i; }
    void _get_record(const IType i);
    void _get_record(const IType i, T* dest);
	uint16_t _get_priority(const IType i);
    void _set_record(const IType i) const;
	void _set_record(const IType i, const T* const src) const;
	void _insert_helper(IType idx);
	void _heapify(IType idx);
};


template <typename T, typename IType, uint32_t MAX_SIZE>
void HeapREU<T, IType, MAX_SIZE>::_get_record(const IType i) {
    reucpy((void *)&(this->record), this->_get_addr(i), this->record_size, REU2RAM);
}

template <typename T, typename IType, uint32_t MAX_SIZE>
void HeapREU<T, IType, MAX_SIZE>::_get_record(const IType i, T* dest) {
    reucpy((void *)dest, this->_get_addr(i), this->record_size, REU2RAM);
}

template <typename T, typename IType, uint32_t MAX_SIZE>
uint16_t HeapREU<T, IType, MAX_SIZE>::_get_priority(const IType i) {
	uint16_t priority;
	reucpy((void *)&priority, this->_get_addr(i), 2, REU2RAM);
	return priority;
}

template <typename T, typename IType, uint32_t MAX_SIZE>
void HeapREU<T, IType, MAX_SIZE>::_set_record(const IType i) const {
    reucpy((void *)&(this->record), this->_get_addr(i), this->record_size, RAM2REU);
}

template <typename T, typename IType, uint32_t MAX_SIZE>
void HeapREU<T, IType, MAX_SIZE>::_set_record(const IType i, const T* const src) const {
    reucpy((void *)src, this->_get_addr(i), this->record_size, RAM2REU);
}

template <typename T, typename IType, uint32_t MAX_SIZE>
void HeapREU<T, IType, MAX_SIZE>::init(const reu_addr_t addr) {
	this->_size = 0;
    this->base_addr = addr;
    this->end_addr = addr + this->memory_size;
}

template <typename T, typename IType, uint32_t MAX_SIZE>
void HeapREU<T, IType, MAX_SIZE>::init(const T* const items, const IType size, const reu_addr_t addr) {
	this->init(addr);
	IType i;
	for (i = 0; i < size; i++)
		reucpy((void *)&(items[i]), this->_get_addr(i), 1, RAM2REU);
	this->_size = i;
	i = (this->_size - 2) >> 1;
	while (i >= 0) {
		this->_heapify(i);
		i--;
	}
}

template <typename T, typename IType, uint32_t MaxSize>
void HeapREU<T, IType, MaxSize>::_insert_helper(IType idx) {
    IType parent_node;
    while (idx > 0 && this->_get_priority(parent_node = (idx - 1) >> 1) > this->_get_priority(idx)) {
        T temp;
		this->_get_record(parent_node, &temp);
        this->_get_record(idx); this->_set_record(parent_node);
		this->_set_record(idx, &temp);
        idx = parent_node;
    }
}

template <typename T, typename IType, uint32_t MaxSize>
void HeapREU<T, IType, MaxSize>::_heapify(IType idx) {
	for (;;) {
		IType left  = (idx << 1) + 1;
		IType right = (idx << 1) + 2;
		IType min = idx;
		if (left  >= this->_size || left < 0)  left = -1;
		if (right >= this->_size || right < 0) right = -1;
		if (left  != -1 && this->_get_priority(left) < this->_get_priority(idx)) min = left;
		if (right != -1 && this->_get_priority(right) < this->_get_priority(min)) min = right;
		if (min == idx) break;
		T temp;
		this->_get_record(min, &temp);
		this->_get_record(idx); this->_set_record(min);
		this->_set_record(idx, &temp);
		idx = min;
	}
}

template <typename T, typename IType, uint32_t MaxSize>
T HeapREU<T, IType, MaxSize>::pop() {
	assert(this->_size > 0);
	T deleteItem;
	this->_get_record(0, &deleteItem);
	this->_get_record(this->_size - 1);
	this->_set_record(0);
	this->_size--;
	this->_heapify(0);
	return deleteItem;
}

template <typename T, typename IType, uint32_t MaxSize>
void HeapREU<T, IType, MaxSize>::popInto(T* const item) {
	assert(this->_size > 0);
	this->_get_record(0, item);
	this->_get_record(this->_size - 1);
	this->_set_record(0);
	this->_size--;
	this->_heapify(0);
}

template <typename T, typename IType, uint32_t MaxSize>
void HeapREU<T, IType, MaxSize>::push(const T* const n) {
	assert(this->_size < MaxSize);
	this->_set_record(this->_size, n);
	this->_insert_helper(this->_size);
	this->_size++;
}

#endif //__MIN_HEAP_REU_H__
