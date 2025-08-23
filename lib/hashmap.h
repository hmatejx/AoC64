#ifndef __HASHMAP_H__
#define __HASHMAP_H__

#include <assert.h>
#include <string.h>
#include <stdint.h>


// The hash set structure
// TKey: must implement IType hash() and the == operator
// IType: integer type to be used for index variables
template <typename TKey, typename TItem, typename IType, uint16_t MAX_SIZE>
struct HashMap {
    // Data
    TKey keys[MAX_SIZE];
    TItem items[MAX_SIZE];
    int8_t marker[MAX_SIZE] = {0};
    // For marking
    static const int8_t EMPTY = 0;
    static const int8_t FULL = 1;
    static const int8_t TOMBSTONE = -1;
    // Custom invalid index value
    static const IType INVALID_INDEX = (IType)-1;

    // members
    // Finds an item by key and returns its index
    IType find(const TKey& key) const;
    // Gets an item by key
    const TItem* get(const TKey& key) const;
    // Inserts a key-item pair
    void insert(const TKey& key, const TItem& item);
    // Removes an item by key
    int8_t remove(const TKey& key);
    // Returns the number of elements
    IType size() const;
    // Clears the map (marker array only for performance)
    void clear() { memset(marker, EMPTY, MAX_SIZE); }

    void print(const char* name) const;
};


template <typename TKey, typename TItem, typename IType, uint16_t MAX_SIZE>
IType HashMap<TKey, TItem, IType, MAX_SIZE>::find(const TKey& key) const {
    const IType i0 = key.hash() % MAX_SIZE;
    IType i = i0;
    do {
        if (marker[i] == FULL && keys[i] == key)
            return i;
        if (marker[i] == EMPTY)
            return INVALID_INDEX;
        if (++i == MAX_SIZE)
            i = 0;
    } while (i != i0);
    return INVALID_INDEX;
}

template <typename TKey, typename TItem, typename IType, uint16_t MAX_SIZE>
const TItem* HashMap<TKey, TItem, IType, MAX_SIZE>::get(const TKey& key) const {
    const IType i = find(key);
    if (i != INVALID_INDEX)
        return &(items[i]);
    return nullptr;
}

template <typename TKey, typename TItem, typename IType, uint16_t MAX_SIZE>
void HashMap<TKey, TItem, IType, MAX_SIZE>::insert(const TKey& key, const TItem& item) {
    const IType i0 = key.hash() % MAX_SIZE;
    IType i = i0;
    IType tombstone = INVALID_INDEX;
    while (marker[i] != EMPTY) {
        if (marker[i] == FULL && keys[i] == key) {
            items[i] = item;
            return;
        }
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

    keys[i] = key;
    items[i] = item;
    marker[i] = FULL;
}

template <typename TKey, typename TItem, typename IType, uint16_t MAX_SIZE>
int8_t HashMap<TKey, TItem, IType, MAX_SIZE>::remove(const TKey& key) {
    const IType i = find(key);
    if (i == INVALID_INDEX)
        return -1;
    marker[i] = TOMBSTONE;
    return 0;
}

template <typename TKey, typename TItem, typename IType, uint16_t MAX_SIZE>
IType HashMap<TKey, TItem, IType, MAX_SIZE>::size() const {
    IType l = 0;
    for (IType i = 0; i < MAX_SIZE; ++i)
        if (marker[i] == FULL)
            ++l;
    return l;
}

#endif //__HASHMAP_H__