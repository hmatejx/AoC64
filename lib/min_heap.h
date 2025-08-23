#ifndef __MIN_HEAP_H__
#define __MIN_HEAP_H__

#include <stdint.h>
#include <assert.h>

// The heap structure.
// The struct T must provide a priority member variable in the first slot:
// 	struct T __attribute__((packed)) {
//   	int priority; // can be any integer type
//	 	...
// 	};
template <typename T, typename IType, uint16_t MAX_SIZE>
struct Heap {
	// Data
	T arr[MAX_SIZE];
	IType _size;
	// Initializes the min heap
	void init();
	void init(const T* const items, const IType n_items);
	// Adds an item to the heap
	void push(const T& item);
	void push(const T&& item);
	// Removes and returns the top element
	T pop();
	// Returns the number of elements
	const IType size() const { return _size; }
	// Clears the heap
	void clear() { _size = 0; }

	// internal helper functions
	void _insert_helper(IType idx);
	void _heapify(IType idx);
};

// implementation
template <typename T, typename IType, uint16_t MAX_SIZE>
void Heap<T, IType, MAX_SIZE>::init() {
	_size = 0;
}

// Warning: this implementation works only for size > 1
template <typename T, typename IType, uint16_t MAX_SIZE>
void Heap<T, IType, MAX_SIZE>::init(const T* const items, const IType size) {
	_size = 0;
	memset(arr, 0, sizeof(T)*size);
	IType i;
	for (i = 0; i < size; i++)
		arr[i] = items[i];
	_size = i;
	i = (_size - 2) >> 1;
	while (i >= 0) {
		_heapify(i);
		i--;
	}
}

template <typename T, typename IType, uint16_t MAX_SIZE>
void Heap<T, IType, MAX_SIZE>::_insert_helper(IType idx) {
    IType parent_node;
    while (idx > 0 && arr[parent_node = (idx - 1) >> 1].priority > arr[idx].priority) {
        const T temp = arr[parent_node];
        arr[parent_node] = arr[idx];
        arr[idx] = temp;
        idx = parent_node;
    }
}

template <typename T, typename IType, uint16_t MAX_SIZE>
void Heap<T, IType, MAX_SIZE>::_heapify(IType idx) {
	for (;;) {
		IType left  = (idx << 1) + 1;
		IType right = left + 1;
		IType min = idx;
		if (left  >= _size || left < 0)
			left = -1;
		if (right >= _size || right < 0)
			right = -1;
		if (left  != -1 && arr[left].priority  < arr[idx].priority)
			min = left;
		if (right != -1 && arr[right].priority < arr[min].priority)
			min = right;
		if (min == idx)
			break;
		const T temp = arr[min];
		arr[min] = arr[idx];
		arr[idx] = temp;
		idx = min;
	}
}

template <typename T, typename IType, uint16_t MaxSize>
T Heap<T, IType, MaxSize>::pop() {
	assert(_size > 0);
	T deleteItem;
	deleteItem = arr[0];
	arr[0] = arr[_size - 1];
	_size--;
	_heapify(0);
	return deleteItem;
}

template <typename T, typename IType, uint16_t MAX_SIZE>
void Heap<T, IType, MAX_SIZE>::push(const T& n) {
	assert(_size < MAX_SIZE);
	arr[_size] = n;
	_insert_helper(_size);
	_size++;
}

template <typename T, typename IType, uint16_t MAX_SIZE>
void Heap<T, IType, MAX_SIZE>::push(const T&& n) {
	assert(_size < MAX_SIZE);
	arr[_size] = n;
	_insert_helper(_size);
	_size++;
}

#endif //__MIN_HEAP_H__
