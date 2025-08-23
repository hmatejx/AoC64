#ifndef __HASHSET_REU_H__
#define __HASHSET_REU_H__

#include <assert.h>
#include <string.h>
#include <stdint.h>
#include "reucpy.h"

// The hash set record structure (auxiliary)
// TKey: must implement IType hash() and the == operator
template <typename TKey>
struct HashSetRecord {
    int8_t marker;
    TKey key;
};

// The hash set structure. Before use call the Init(base_addr) method
// Tkey: must implement IType hash() and the == operator
// IType: integer type to be used for index variables
// base_addr: should be aligned on 64KB
template <typename TKey, typename IType, uint32_t MAX_SIZE>
struct HashSetREU {
    // Data
    reu_addr_t base_addr;
    reu_addr_t end_addr;
    HashSetRecord<TKey> record;
    // For memory sizes
    static const uint16_t record_size = (uint16_t)sizeof(HashSetRecord<TKey>);
    static const reu_addr_t memory_size = (reu_addr_t)(MAX_SIZE*record_size);
    // Used for marking
    static const int8_t EMPTY = 0;
    static const int8_t FULL = 1;
    static const int8_t TOMBSTONE = -1;
    // Custom invalid index value
    static const IType INVALID_INDEX = (IType)-1;
    // For iteration
    IType iter_index;

    // Initializes the set on the REU, needs to be called before first use
    void init(reu_addr_t addr);
    // Finds an item and returns its index
    IType find(const TKey& key);
    // Inserts an item
    void insert(const TKey& key);
    // Removes an item
    int8_t remove(const TKey& key);
    // Returns the number of elements
    IType size();
    // Checks if record i in the REU is occupied
    int8_t isOccupied(IType i) { this->_get_record_marker(i); return this->record.marker == FULL; }
    // clears the REU memory in 64KB blocks (potential for spill over!)
    void clear() const;
    // Returns the first element
    const TKey* first();
    // Returns the next element
    const TKey* next();

    // Internal helpers, can also be used for performance hacks
    reu_addr_t _get_addr(const IType i) const { return this->base_addr + (reu_addr_t)this->record_size*i; }
    void _get_record(const IType i);
    void _set_record(const IType i) const;
    void _get_record_marker(const IType i);
    void _set_record_marker(const IType i) const;
};


template <typename TKey, typename IType, uint32_t MAX_SIZE>
void HashSetREU<TKey, IType, MAX_SIZE>::init(reu_addr_t addr) {
    this->base_addr = addr;
    this->end_addr = addr + this->memory_size;
    this->clear();
};

template <typename TKey, typename IType, uint32_t MAX_SIZE>
void HashSetREU<TKey, IType, MAX_SIZE>::clear() const {
    for (reu_addr_t addr = this->base_addr; addr < this->end_addr; addr += (reu_addr_t)0x10000)
        reuset((reu_addr_t)addr, 0, 0);
};

template <typename TKey, typename IType, uint32_t MAX_SIZE>
IType HashSetREU<TKey, IType, MAX_SIZE>::find(const TKey& key) {
    const IType i0 = key.hash() % MAX_SIZE;
    IType i = i0;
    do {
        this->_get_record(i);
        if (this->record.marker == FULL && this->record.key == key)
            return i;
        if (this->record.marker == EMPTY)
            return INVALID_INDEX;
        if (++i == MAX_SIZE)
            i = 0;
    } while (i != i0);
    return INVALID_INDEX;
}

template <typename TKey, typename IType, uint32_t MAX_SIZE>
void HashSetREU<TKey, IType, MAX_SIZE>::insert(const TKey& key) {
    const IType i0 = key.hash() % MAX_SIZE;
    IType i = i0;
    IType tombstone = INVALID_INDEX;
    this->_get_record(i);
    while (this->record.marker != EMPTY) {
        if (this->record.marker == FULL && this->record.key == key)
            return;
        if (this->record.marker == TOMBSTONE && tombstone == INVALID_INDEX)
            tombstone = i;
        if (++i == MAX_SIZE)
            i = 0;
        if (i == i0) {
            assert(tombstone != INVALID_INDEX);
            break;
        }
        this->_get_record(i);
    }
    if (tombstone != INVALID_INDEX)
        i = tombstone;

    this->record.marker = FULL;
    this->record.key = key;
    this->_set_record(i);
}

template <typename TKey, typename IType, uint32_t MAX_SIZE>
int8_t HashSetREU<TKey, IType, MAX_SIZE>::remove(const TKey& key) {
    const IType i = this->find(key);
    if (i == INVALID_INDEX)
        return -1;
    this->record.marker = TOMBSTONE;
    this->_set_record_marker(i);
    return 0;
}

template <typename TKey, typename IType, uint32_t MAX_SIZE>
IType HashSetREU<TKey, IType, MAX_SIZE>::size() {
    IType l = 0;
    for (IType i = 0; i < MAX_SIZE; ++i) {
        if (this->_get_record_marker(i) == FULL)
            ++l;
    }
    return l;
}

template <typename TKey, typename IType, uint32_t MAX_SIZE>
void HashSetREU<TKey, IType, MAX_SIZE>::_get_record(const IType i) {
    reucpy((void *)&(this->record), this->_get_addr(i), this->record_size, REU2RAM);
}

template <typename TKey, typename IType, uint32_t MAX_SIZE>
void HashSetREU<TKey, IType, MAX_SIZE>::_set_record(const IType i) const {
    reucpy((void *)&(this->record), this->_get_addr(i), this->record_size, RAM2REU);
}

template <typename TKey, typename IType, uint32_t MAX_SIZE>
void HashSetREU<TKey, IType, MAX_SIZE>::_get_record_marker(const IType i) {
    reucpy((void *)&this->record.marker, this->_get_addr(i), 1, REU2RAM);
}

template <typename TKey, typename IType, uint32_t MAX_SIZE>
void HashSetREU<TKey, IType, MAX_SIZE>::_set_record_marker(const IType i) const {
    reucpy((void *)&this->record.marker, this->_get_addr(i), 1, RAM2REU);
}

template <typename TKey, typename IType, uint32_t MAX_SIZE>
const TKey* HashSetREU<TKey, IType, MAX_SIZE>::first() {
    for (this->iter_index = 0; this->iter_index < MAX_SIZE; ++this->iter_index) {
        this->_get_record(this->iter_index);
        if (this->record.marker == FULL) {
            return &this->record.key;
        }
    }
    return nullptr;
}

template <typename TKey, typename IType, uint32_t MAX_SIZE>
const TKey* HashSetREU<TKey, IType, MAX_SIZE>::next() {
    for (++this->iter_index; this->iter_index < MAX_SIZE; ++this->iter_index) {
        this->_get_record(this->iter_index);
        if (this->record.marker == FULL) {
            return &this->record.key;
        }
    }
    return nullptr;
}

#endif //__HASHSET_REU_H__