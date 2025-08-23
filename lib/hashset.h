#ifndef __HASHSET_H__
#define __HASHSET_H__

#include <assert.h>
#include <string.h>
#include <stdint.h>

// The hash set structure.
// T: must implement IType hash() and the == operator
// IType: integer type to be used for index variables
template <typename T, typename IType, uint16_t MAX_SIZE>
struct HashSet {
    // Data
    T data[MAX_SIZE];
    int8_t marker[MAX_SIZE];
    // Custom invalid index value
    static const IType INVALID_INDEX = (IType)-1;
    // For marking
    static const int8_t EMPTY = 0;
    static const int8_t FULL = 1;
    static const int8_t TOMBSTONE = -1;
    // For iteration
    IType iter_index;

    // Finds an item and returns its index
    IType find(const T& item) const;
    // Inserts an item
    void insert(const T& item);
    // Removes an item
    int8_t remove(const T& item);
    // Returns the number of elements
    IType size() const;
    // Checks if position i in the backing array is occupied
    int8_t isOccupied(const IType i) const { return marker[i] == FULL; }
    // Clears the set
    void clear() { memset(marker, EMPTY, MAX_SIZE); this->iter_index = INVALID_INDEX; }
    // Returns the first element
    const T* first();
    // Returns the next element
    const T* next();
};


template <typename T, typename IType, uint16_t MAX_SIZE>
IType HashSet<T, IType, MAX_SIZE>::find(const T& item) const {
    const IType i0 = item.hash() % MAX_SIZE;
    IType i = i0;
    do {
        if (marker[i] == FULL & data[i] == item)
            return i;
        if (marker[i] == EMPTY)
            return INVALID_INDEX;
        if (++i == MAX_SIZE)
            i = 0;
    } while (i != i0);
    return INVALID_INDEX;
}

template <typename T, typename IType, uint16_t MAX_SIZE>
void HashSet<T, IType, MAX_SIZE>::insert(const T& item) {
    const IType i0 = item.hash() % MAX_SIZE;
    IType i = i0;
    IType tombstone = INVALID_INDEX;
    while (marker[i] != EMPTY) {
        if (marker[i] == FULL && data[i] == item)
            return;
        if (marker[i] == TOMBSTONE && tombstone == INVALID_INDEX)
            tombstone = i;
        if (++i == MAX_SIZE)
            i = 0;
        if (i == i0) {
            assert(tombstone != INVALID_INDEX);
            break;
        }
    }
    if (tombstone != INVALID_INDEX)
        i = tombstone;

    data[i] = item;
    marker[i] = FULL;
}

template <typename T, typename IType, uint16_t MAX_SIZE>
int8_t HashSet<T, IType, MAX_SIZE>::remove(const T& item) {
    const IType i = this->find(item);
    if (i == INVALID_INDEX)
        return -1;
    marker[i] = TOMBSTONE;
    return 0;
}

template <typename T, typename IType, uint16_t MAX_SIZE>
IType HashSet<T, IType, MAX_SIZE>::size() const {
    IType l = 0;
    for (IType i = 0; i < MAX_SIZE; ++i)
        if (this->marker[i] == FULL)
            ++l;
    return l;
}

template <typename T, typename IType, uint16_t MAX_SIZE>
const T* HashSet<T, IType, MAX_SIZE>::first() {
    for (this->iter_index = 0; this->iter_index < MAX_SIZE; ++this->iter_index) {
        if (this->marker[this->iter_index] == FULL)
            return &this->data[this->iter_index];
    }
    return nullptr;
}

template <typename T, typename IType, uint16_t MAX_SIZE>
const T* HashSet<T, IType, MAX_SIZE>::next() {
    for (++this->iter_index; this->iter_index < MAX_SIZE; ++this->iter_index) {
        if (this->marker[this->iter_index] == FULL)
            return &this->data[this->iter_index];
    }
    return nullptr;
}

#endif //__HASHSET_H__