#ifndef __HASHMAP_REU_H__
#define __HASHMAP_REU_H__

#include <assert.h>
#include <string.h>
#include <stdint.h>
#include "reucpy.h"

// the has record structure
// TKey: must implement IType hash() and the == operator
// IType: integer type to be used for index variables
template <typename TKey, typename TItem>
struct HashMapRecord {
    int8_t marker;
    TKey key;
    TItem item;
};


// the hash set structure
// TKey: must implement IType hash() and the == operator
// IType: integer type to be used for index variables
// before use call the Init(base_addr) member function
template <typename TKey, typename TItem, typename IType, uint32_t MAX_SIZE>
struct HashMapREU {
    // Data
    reu_addr_t base_addr;
    reu_addr_t end_addr;
    HashMapRecord<TKey, TItem> record;
    // Memory sizes
    static const uint16_t record_size = (uint16_t)sizeof(HashMapRecord<TKey, TItem>);
    static const reu_addr_t memory_size = (reu_addr_t)MAX_SIZE*record_size;
    // For marking
    static const int8_t EMPTY = 0;
    static const int8_t FULL = 1;
    static const int8_t TOMBSTONE = -1;
    // Custom invalid index value
    static const IType INVALID_INDEX = (IType)-1;
    // For iteration
    IType iter_index;

    // Initializes the map on the REU, needs to be called before first use
    void init(reu_addr_t addr);
    // Finds an item by key and returns its index
    IType find(const TKey& key);
    // Gets an item by key
    const TItem* get(const TKey& key);
    // Inserts (or overwrites) a key-item pair
    void insert(const TKey& key, const TItem& item);
    // Removes an item by key
    int8_t remove(const TKey& key);
    // Returns the number of elements
    IType size();
    // clears the REU memory in 64KB blocks (potential for spill over!)
    void clear() const;
    // Returns the first element
    const HashMapRecord<TKey, TItem>* first();
    // Returns the next element
    const HashMapRecord<TKey, TItem>* next();

    // Internal helpers, can also be used for performance hacks
    reu_addr_t _get_addr(const IType i) const { return base_addr + (reu_addr_t)record_size*i; }
    void _get_record(const IType i);
    void _set_record(const IType i) const;
    void _get_record_marker(const IType i);
    void _set_record_marker(const IType i) const;
};


template <typename TKey, typename TItem, typename IType, uint32_t MAX_SIZE>
void HashMapREU<TKey, TItem, IType, MAX_SIZE>::init(reu_addr_t addr) {
    base_addr = addr;
    end_addr = addr + memory_size;
    clear();
};

template <typename TKey, typename TItem, typename IType, uint32_t MAX_SIZE>
void HashMapREU<TKey, TItem, IType, MAX_SIZE>::clear() const {
    for (reu_addr_t addr = base_addr; addr < end_addr; addr += (reu_addr_t)0x10000)
        reuset((reu_addr_t)addr, 0, 0);
};

template <typename TKey, typename TItem, typename IType, uint32_t MAX_SIZE>
IType HashMapREU<TKey, TItem, IType, MAX_SIZE>::find(const TKey& key) {
    const IType i0 = key.hash() % MAX_SIZE;
    IType i = i0;
    do {
        _get_record(i);
        if (record.marker == FULL && record.key == key)
            return i;
        if (record.marker == EMPTY)
            return INVALID_INDEX;
        if (++i == MAX_SIZE)
            i = 0;
    } while (i != i0);
    return INVALID_INDEX;
}

// pointer to an internal record; make sure to create a copy of the value if needed!
template <typename TKey, typename TItem, typename IType, uint32_t MAX_SIZE>
const TItem* HashMapREU<TKey, TItem, IType, MAX_SIZE>::get(const TKey& key) {
    const IType i = find(key);
    if (i != INVALID_INDEX)
        return &(record.item);
    return nullptr;
}

template <typename TKey, typename TItem, typename IType, uint32_t MAX_SIZE>
void HashMapREU<TKey, TItem, IType, MAX_SIZE>::insert(const TKey& key, const TItem& item) {
    const IType i0 = key.hash() % MAX_SIZE;
    IType i = i0;
    IType tombstone = INVALID_INDEX;
    _get_record(i);
    while (record.marker != EMPTY) {
        if (record.marker == FULL && record.key == key) {
            record.item = item;
            _set_record(i);
            return;
        }
        if (record.marker == TOMBSTONE && tombstone == INVALID_INDEX)
            tombstone = i;
        if (++i == MAX_SIZE)
            i = 0;
        if (i == i0) {
            assert(tombstone != INVALID_INDEX);
            break;
        }
        _get_record(i);
    }
    if (tombstone != INVALID_INDEX)
        i = tombstone;

    record.marker = FULL;
    record.key = key;
    record.item = item;
    _set_record(i);
}

template <typename TKey, typename TItem, typename IType, uint32_t MAX_SIZE>
int8_t HashMapREU<TKey, TItem, IType, MAX_SIZE>::remove(const TKey& key) {
    const IType i = find(key);
    if (i == INVALID_INDEX)
        return -1;

    record.marker = TOMBSTONE;
    _set_record(i);
    return 0;
}

template <typename TKey, typename TItem, typename IType, uint32_t MAX_SIZE>
IType HashMapREU<TKey, TItem, IType, MAX_SIZE>::size() {
    IType l = 0;
    for (IType i = 0; i < MAX_SIZE; ++i) {
        _get_record(i);
        if (record.marker == FULL)
            ++l;
    }
    return l;
}

template <typename TKey, typename TItem, typename IType, uint32_t MAX_SIZE>
void HashMapREU<TKey, TItem, IType, MAX_SIZE>::_get_record(const IType i) {
    reucpy((void *)&(record), _get_addr(i), record_size, REU2RAM);
}

template <typename TKey, typename TItem, typename IType, uint32_t MAX_SIZE>
void HashMapREU<TKey, TItem, IType, MAX_SIZE>::_set_record(const IType i) const {
    reucpy((void *)&(record), _get_addr(i), record_size, RAM2REU);
}

template <typename TKey, typename TItem, typename IType, uint32_t MAX_SIZE>
void HashMapREU<TKey, TItem, IType, MAX_SIZE>::_get_record_marker(const IType i) {
    reucpy((void *)&(record.marker), _get_addr(i), 1, REU2RAM);
}

template <typename TKey, typename TItem, typename IType, uint32_t MAX_SIZE>
void HashMapREU<TKey, TItem, IType, MAX_SIZE>::_set_record_marker(const IType i) const {
    reucpy((void *)&(record.marker), _get_addr(i), 1, RAM2REU);
}

template <typename TKey, typename TItem, typename IType, uint32_t MAX_SIZE>
const HashMapRecord<TKey, TItem>* HashMapREU<TKey, TItem, IType, MAX_SIZE>::first() {
    for (iter_index = 0; iter_index < MAX_SIZE; ++iter_index) {
        _get_record(iter_index);
        if (record.marker == FULL) {
            return &record;
        }
    }
    return nullptr;
}

template <typename TKey, typename TItem, typename IType, uint32_t MAX_SIZE>
const HashMapRecord<TKey, TItem>* HashMapREU<TKey, TItem, IType, MAX_SIZE>::next() {
    for (++iter_index; iter_index < MAX_SIZE; ++iter_index) {
        _get_record(iter_index);
        if (record.marker == FULL) {
            return &record;
        }
    }
    return nullptr;
}

#endif //__HASHMAP_REU_H__