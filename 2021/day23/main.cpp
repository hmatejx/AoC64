#define NDEBUG

#include <stdint.h>
#include "AoC64.h"
#include "stack.h"
#include "min_heap_reu.h"
#include "hashmap_reu.h"
#include "pearson.h"
#include "math.h"
#if __has_include("input.h")
  #include "input.h"          // user’s private input (gitignored)
#else
  #include "input.example.h"  // checked-in sample input
#endif

uint8_t puzzle_size;
uint8_t room_size;

// the puzzle state represented as a string
// "...........ABCDABCD" (in solved state)
struct Puzzle {
    // puzzle state
    char str[28];
    // constructors, operators, and member functions required by the helper library
    Puzzle() : str() {}
    Puzzle(const char* const other_str) { strcpy(str, other_str); }
    Puzzle(Puzzle& other) { strcpy(str, other.str); }
    Puzzle(const Puzzle& other) { strcpy(str, other.str); }
    bool operator==(Puzzle& other) { return strcmp(str, other.str) == 0; }
    bool operator==(const Puzzle& other) const { return strcmp(str, other.str) == 0; }
    reu_addr_t hash() const { return hash24((uint8_t *)str, puzzle_size); }
};
// structure to be used for the min heap for Dijkstra
struct Item {
    uint16_t priority;
    Puzzle puzzle;
};

// puzzle definition constants
const uint16_t energy[4] = {1, 10, 100, 1000};
const uint8_t parking[7] = {0, 1, 3, 5, 7, 9, 10}; // parking positions
const uint8_t stepout[4] = {2, 4, 6, 8}; // room entrance positions
const uint8_t target[4][4] = {{11, 15, 19, 23}, {12, 16, 20, 24}, {13, 17, 21, 25}, {14, 18, 22, 26}}; // target indices for each amphi
const uint8_t targetI[27] = {0,0,0,0,0,0,0,0,0,0,0,0,1,2,3,0,1,2,3,0,1,2,3,0,1,2,3};  // inverse mapping for the target

// convert the amphipod char to index (potential for minor optimisation...)
inline uint8_t idx(const char chr) { return chr - 'a'; }

// global variables
Stack<uint8_t, uint8_t, 8> _parking;
struct Move { uint8_t a, b; };
Stack<Move, uint8_t, 32> _moves;
static Item _item;


void read_puzzle(Puzzle& puzzle, uint8_t part2 = 0) {
    memset(&puzzle.str[0], '.', 11);
    for (uint8_t i = 0; i < 2; ++i)
        for (uint8_t j = 0; j < 4; ++j)
            puzzle.str[11 + (part2 ? 12 : 4)*i + j] = input[i + 2][3 + 2*j] + 'a' - 'A';
    if (!part2) {
        room_size = 2;
        puzzle_size = 20;
        puzzle.str[19] = 0;
    } else {
        room_size = 4;
        puzzle_size = 28;
        puzzle.str[15] = 'd'; puzzle.str[16] = 'c'; puzzle.str[17] = 'b'; puzzle.str[18] = 'a';
        puzzle.str[19] = 'd'; puzzle.str[20] = 'b'; puzzle.str[21] = 'a'; puzzle.str[22] = 'c';
        puzzle.str[27] = 0;
    }
}


inline uint8_t blocked(const uint8_t a, const uint8_t b, const Puzzle& puzzle) {
    int8_t start, end;
    if (a < b) { start = a+1; end = b+1; } else { start = b; end = a; }
    for (uint8_t pos = start; pos < end; ++pos)
        if (puzzle.str[pos] != '.')
            return 1;
    return 0;
}

// modifies _parking
inline void get_possible_parc_pos(const uint8_t a, const Puzzle& puzzle) {
    _parking.clear();
    for (uint8_t i = 0; i < sizeof(parking); ++i) {
        const uint8_t b = parking[i];
        if (puzzle.str[b] == '.' && !blocked(a, b, puzzle))
            _parking.push(b);
    }
}

// modifies _item.puzzle
inline void make_move(const uint8_t a, const uint8_t b, const Puzzle& puzzle) {
    memcpy_reu(_item.puzzle.str, puzzle.str, puzzle_size);
    const char tmp = _item.puzzle.str[a];
    _item.puzzle.str[a] = _item.puzzle.str[b];
    _item.puzzle.str[b] = tmp;
}


inline uint8_t can_leave_room(const char room, const Puzzle& puzzle) {
    const uint8_t* targets = target[idx(room)];
    uint8_t i;
    for (i = 0; i < room_size; ++i) {
        const uint8_t pos = targets[i];
        if (puzzle.str[pos] != room && puzzle.str[pos] != '.')
            break;
    }
    if (i == room_size)
        return 0;
    for (i = 0; i < room_size; ++i) {
        const uint8_t a = targets[i];
        if (puzzle.str[a] == '.')
            continue;
        return a;
    }
    return 0;
}


inline uint8_t can_enter_room(const uint8_t a, const uint8_t b, const char amphi, const Puzzle& puzzle) {
    const uint8_t* targets = target[idx(amphi)];
    uint8_t best_pos;
    for (uint8_t i = 0; i < room_size; ++i) {
        const uint8_t pos = targets[i];
        if (puzzle.str[pos] == '.')
            best_pos = pos;
        else if (puzzle.str[pos] != amphi)
            return 0;
    }
    return blocked(a, b, puzzle) ? 0 : best_pos;
}


// fills _moves
inline void possible_moves(const Puzzle& puzzle) {
    _moves.clear();
    Move m;
    for (uint8_t i = 0; i < sizeof(parking); ++i) {
        m.a = parking[i];
        if (puzzle.str[m.a] == '.')
            continue;
        char amphi = puzzle.str[m.a];
        m.b = can_enter_room(m.a, stepout[idx(amphi)], amphi, puzzle);
        if (m.b)
            _moves.push(m);
    }
    for (char room = 'a'; room <= 'd'; ++room) {
        m.a = can_leave_room(room, puzzle);
        if (!m.a)
            continue;
        get_possible_parc_pos(stepout[idx(room)], puzzle); // -> _parking
        const uint8_t len = _parking.size();
        for (uint8_t i = 0; i < len; ++i) {
            m.b = _parking.arr[i];
            _moves.push(m);
        }
        _parking.clear();
    }
}


uint16_t solve(const Puzzle& puzzle) {
    Puzzle solution;
    if (puzzle_size > 20)
        solution = "...........abcdabcdabcdabcd";
    else
        solution = "...........abcdabcd";
    Item item {0, puzzle};
    // open and closed set ( needs 16MB REU :O )
    constexpr reu_addr_t offset = 0x100; // skip the bottom 256 bytes of the REU
    constexpr reu_addr_t open_size = 0x5000;
    constexpr reu_addr_t closed_size = 0x40000;
    HeapREU<Item, uint16_t, open_size> open;
    HashMapREU<Puzzle, uint16_t, reu_addr_t, closed_size> closed;
    // push initial state to the open set
    open.init(offset);
    open.push(&item);
    closed.init(offset + open_size*sizeof(open.record));
    // Dijkstra
    uint8_t t = 0;
    while (open.size()) {
        item = open.pop();
        if (item.puzzle == solution)
            return item.priority;
        possible_moves(item.puzzle);
        while (_moves.size()) {
            Move m = _moves.pop();
            uint8_t parking, room_pos;
            if (m.a < m.b) { parking = m.a; room_pos = m.b; } else { parking = m.b; room_pos = m.a; }
            make_move(m.a, m.b, item.puzzle); // modifies _item.puzzle
            const uint8_t distance = ABS(stepout[targetI[room_pos]] - parking) + ((room_pos-7) >> 2);
            const uint16_t new_priority = item.priority + (uint16_t)distance*energy[idx(item.puzzle.str[m.a])];
            const uint16_t* seen_priority = closed.get(_item.puzzle);
            if (seen_priority != nullptr && new_priority >= *seen_priority)
                continue;
            closed.insert(_item.puzzle, new_priority);
            _item.priority = new_priority;
            open.push(&_item);
        }
        tick((++t >> 1) & (uint8_t)7);
    }

    return 0;
}


int main(void) {
    init(23);

    Puzzle puzzle;
    read_puzzle(puzzle);
    printf("part 1: %u\n", solve(puzzle));
    read_puzzle(puzzle, 1);
    printf("part 2: %u\n", solve(puzzle));

    finish();
}