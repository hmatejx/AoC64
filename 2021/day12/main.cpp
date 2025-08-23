#include <stdio.h>
#include <string.h>
#include "AoC64.h"
#include "stack.h"
#if __has_include("input.h")
  #include "input.h"          // user’s private input (gitignored)
#else
  #include "input.example.h"  // checked-in sample input
#endif

inline int8_t min(const int8_t a, const int8_t b) { return a > b ? b : a; }

// memory limits for global variables on stack
constexpr int8_t MAX_CAVES = 16;
constexpr int8_t MAX_VISITED = 10;
constexpr uint16_t MAX_STACK = 32;

// caves
static struct Caves {
    int8_t count = 0;
    char name[MAX_CAVES][3] = {{}};
} caves;

inline int8_t get_cave(const char* name) {
    for (int8_t i = 0; i < caves.count; ++i)
        if (strncmp(caves.name[i], name, 2) == 0)
            return i;
    return -1;
}

int8_t add_cave(const char* name) {
    for (int8_t i = 0; i < caves.count; ++i)
        if (strncmp(caves.name[i], name, 2) == 0)
            return i;
    caves.name[caves.count  ][0] = name[0];
    caves.name[caves.count++][1] = name[1];
    return caves.count - 1;
}

inline int8_t is_small(const int8_t id) {
    return caves.name[id][0] >= 'a';
}

// cave graph
static int8_t graph[MAX_CAVES][MAX_CAVES];
static int8_t start, end;
void setup_graph() {
    char node[3] = {0};
    for (int8_t i = 0; i < n_connections; ++i) {
        char* split = strchr(connections[i], '-');
        memset(node, 0, 3);
        strncpy(node, connections[i], min(split - connections[i], 2));
        const int8_t i1 = add_cave(node);
        memset(node, 0, 3);
        strncpy(node, split + 1, min(strlen(split + 1), 2));
        const int8_t i2 = add_cave(node);
        graph[i1][i2] = 1;
        graph[i2][i1] = 1;
    }
    start = get_cave("st");
    end = get_cave("en");
}

int8_t max_visited = -1;
typedef struct {
    int8_t id;
    int8_t n_visited;
    int8_t twice;
    int8_t visited[MAX_VISITED];
} State;

// global stack


inline int8_t visited(const State* const state, const int8_t id) {
    uint8_t count = 0;
    for (int8_t i = 0; i < state->n_visited; ++i)
        if (state->visited[i] == id)
            ++count;
    return count;
}

inline void visit(State* const state, const int8_t id) {
    state->id = id;
    if (is_small(id)) {
        state->visited[state->n_visited] = id;
        state->n_visited++;
    }
}

inline int8_t can_visit(const State* const state, const int8_t id) {
    if (id == start || id == state->id || !graph[state->id][id]) return 0;
    if (id == end) return 1;
    if (is_small(id)) {
        const int8_t visit_count = visited(state, id);
        if (visit_count == 1 && state->twice == 0) return 2;
        if (visit_count > 0) return 0;
    }
    return 1;
}

int32_t enumerate(const int8_t part2 = 0) {
    int32_t count = 0;
    static Stack<State, uint8_t, MAX_STACK> stack;
    stack.clear();
    stack.push(State {.id = start, .n_visited = 0, .twice = 0, .visited = {}});
    int8_t i = 0;
    while (stack.size()) {
        tick(i++ >> 5 & 7);
        const State current = stack.pop();
        if (current.id == end) {
            ++count;
            continue;
        }
        for (int8_t i = 0; i < caves.count; ++i) {
            const int8_t cave_valid = can_visit(&current, i);
            if (part2 && cave_valid == 2) {
                State new_state = current;
                visit(&new_state, i);
                new_state.twice = 1;
                stack.push(new_state);
            } else if (cave_valid == 1) {
                State new_state = current;
                visit(&new_state, i);
                stack.push(new_state);
            }
        }
    }
    return count;
}

int main(void) {
    init(12);

    setup_graph();
    printf("part 1: %ld\n", enumerate());
    printf("part 2: %ld\n", enumerate(1));

    finish();
}
