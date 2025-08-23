#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "AoC64.h"
#include "min_heap.h"
#include "reucpy.h"
#if __has_include("input.h")
  #include "input.h"          // user’s private input (gitignored)
#else
  #include "input.example.h"  // checked-in sample input
#endif

// size of the open set for Dijkstra using priority queue
// the closed set is living on the REU (starting at address 0)
constexpr int16_t open_size = 2048;
// grid expansion for part 2
constexpr uint8_t multipy = 5;

struct Point {
    int16_t y;
    int16_t x;

    const Point up()    const { return Point {.y = (y-1), .x = x}; }
    const Point down()  const { return Point {.y = (y+1), .x = x}; }
    const Point left()  const { return Point {.y = y, .x = (x-1)}; }
    const Point right() const { return Point {.y = y, .x = (x+1)}; }
    int8_t equal(const Point& other) const { return y == other.y && x == other.x; }
};

struct Node {
    int16_t priority;
    Point p;
};

// open set
Heap<Node, int16_t, open_size> open;


inline uint8_t is_closed(const Point& p, const int16_t dim) {
    int8_t data;
    reucpy((void *)&data, (reu_addr_t)dim * p.y + p.x, 1, REU2RAM);
    return data;
}

inline void set_closed(const Point& p, const int16_t dim) {
    const uint8_t data[1] = { 0xFF };
    reucpy((void *)data, (reu_addr_t)dim * p.y + p.x, 1, RAM2REU);
}

inline int8_t risk_level(const Point& p) {
    const int8_t tx = p.x / base_dim;
    const int8_t ty = p.y / base_dim;
    return 1 + (risk[p.y % base_dim][p.x % base_dim] + tx + ty - 1) % 9;
}

inline void explore(const Point& p, const int16_t priority, const int16_t dim) {
    if (p.x < 0 || p.x >= dim || p.y < 0 || p.y >= dim || is_closed(p, dim))
        return;
    set_closed(p, dim);
    open.push(Node {.priority = priority + risk_level(p), .p = p});
}

int16_t find_path(const int8_t mult) {

    const int16_t mdim = mult*base_dim;
    const Point start = Point {.y = 0, .x = 0};
    const Point end = Point {.y = mdim - 1, .x = mdim - 1};

    const Node n0 = {.priority = 0, .p = start};
    open.init(&n0, 1);
    set_closed(start, mdim);

    int32_t iter = 0;
    while (open.size()) {
        tick((iter >> 4) & (uint8_t)7);
        Node curr = open.pop();
        if (curr.p.equal(end))
            return curr.priority;
        explore(curr.p.left(),  curr.priority, mdim);
        explore(curr.p.right(), curr.priority, mdim);
        explore(curr.p.up(),    curr.priority, mdim);
        explore(curr.p.down(),  curr.priority, mdim);
        ++iter;
    }
    return 0;
}


int main(void) {
    init(15);

    // needs REU
    reu_init();
    const uint16_t nbanks = banks_needed((uint32_t)multipy*multipy*base_dim*base_dim);

    reu_clear(nbanks);
    printf("part 1: %d\n", find_path(1));

    reu_clear(nbanks);
    printf("part 2: %d\n", find_path(multipy));

    finish();
}
