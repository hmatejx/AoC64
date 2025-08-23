#include "AoC64.h"
#include "math.h"
#include "hashset_reu.h"
#include "hashset.h"
#include "pearson.h"
#include <stdint.h>
#include <string.h>
#if __has_include("input.h")
  #include "input.h"          // user’s private input (gitignored)
#else
  #include "input.example.h"  // checked-in sample input
#endif

constexpr uint16_t HASHMAP_SIZE = 512;
constexpr uint16_t HASHMAP_SIZE_REU = 10240;
constexpr uint8_t RANGE_SIZE = 6*sizeof(int24_t);


struct Cuboid {
    int8_t sign;
    int24_t r[6];

    const int64_t volume() const { return (int64_t)sign*(r[1]-r[0]+1)*(r[3]-r[2]+1)*(r[5]-r[4]+1); }
    const int16_t hash() const { return hash16((uint8_t *)&(r[0]), RANGE_SIZE); }
    const int8_t intersection(const Cuboid& other, Cuboid* inter) const;
};

bool operator==(const Cuboid& lhs, const Cuboid& rhs) { return memcmp(&lhs.r[0], &rhs.r[0], RANGE_SIZE) == 0; }


const int8_t Cuboid::intersection(const Cuboid& other, Cuboid* inter) const {

    int24_t o[6] = { MAX(r[0], other.r[0]), MIN(r[1], other.r[1]),
                     MAX(r[2], other.r[2]), MIN(r[3], other.r[3]),
                     MAX(r[4], other.r[4]), MIN(r[5], other.r[5]) };
    // no intersection
    if (o[0] > o[1] || o[2] > o[3] || o[4] > o[5])
        return 0;
    // set the intersection
    inter->sign = other.sign == 1 ? (int8_t)-1 : (int8_t)1;
    memcpy(inter->r, &o, RANGE_SIZE);
    return 1;
}


void parseCuboid(const char* str, Cuboid* c) {
    c->sign = (str[1] == 'n') ? (int8_t)1 : (int8_t)-1;
    int32_t x0, x1, y0, y1, z0, z1;
    sscanf(str, " %*s x=%ld..%ld,y=%ld..%ld,z=%ld..%ld", &x0, &x1, &y0, &y1, &z0, &z1);
    c->r[0] = x0; c->r[1] = x1;
    c->r[2] = y0; c->r[3] = y1;
    c->r[4] = z0; c->r[5] = z1;
}


int main(void) {
    init(22);

    // hash map of cuboids to merge into cuboids after each step
    HashSet<Cuboid, uint16_t, HASHMAP_SIZE> add_cuboids;
    // hash map to hold all cuboids generated in previous steps (on the REU)
    HashSetREU<Cuboid, uint16_t, HASHMAP_SIZE_REU> cuboids;
    reu_init(); cuboids.init(1024); // initialize the the hash map on the REU

    Cuboid new_c, inter;
    uint16_t t = 0;
    for (uint16_t s = 0; s < nsteps; ++s) {
        tick(t++ & (uint8_t)7);
        add_cuboids.clear();
        // construct the new cuboid
        parseCuboid(steps[s], &new_c);
        // if the new cuboid is of the 'on' type, add it as well
        if (new_c.sign > 0)
            add_cuboids.insert(new_c);
        // find intersections with existing cuboids and add them with the right sign
        for (const Cuboid* c = cuboids.first(); c != nullptr; c = cuboids.next()) {
            if (c->sign == 0) { // remove any cuboids that have total multiplicity of 0 from the REU hash set
                cuboids.record.marker = cuboids.TOMBSTONE;
                cuboids._set_record_marker(cuboids.iter_index); // hack, tombstone the current item in place
                continue;
            }
            if (!new_c.intersection(*c, &inter)) // skip cuboids without intersection
                continue;
            uint16_t i = add_cuboids.find(inter);
            if (i == add_cuboids.INVALID_INDEX) {
                add_cuboids.insert(inter);
            } else {
                add_cuboids.data[i].sign += inter.sign;
            }
        }
        // finally, consolidate newly constructed cuboids in add_cuboids back into cuboids
        for (const Cuboid* c = add_cuboids.first(); c != nullptr; c = add_cuboids.next()) {
            uint16_t i = cuboids.find(*c);
            if (i == cuboids.INVALID_INDEX) {
                cuboids.insert(*c);
            } else {
                cuboids.record.key.sign += c->sign;
                cuboids._set_record(i);
            }
        }
    }

    int64_t part1 = 0, part2 = 0;
    for (const Cuboid* c = cuboids.first(); c != nullptr; c = cuboids.next()) {
        const int64_t v = c->volume();
        part2 += v;
        if (c->r[0] < -50 || c->r[0] > 50 || c->r[1] < -50 || c->r[1] > 50 ||
            c->r[2] < -50 || c->r[2] > 50 || c->r[3] < -50 || c->r[3] > 50 ||
            c->r[4] < -50 || c->r[4] > 50 || c->r[5] < -50 || c->r[5] > 50)
            continue;
        part1 += v;
    }

    printf("part 1: %lld\n", part1);
    printf("part 2: %lld\n", part2);

    finish();
}