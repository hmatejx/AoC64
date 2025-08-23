#include <stdio.h>
#include "AoC64.h"
#include "min_heap.h"
#include "hashset.h"
#include "pearson.h"
#include "math.h"
#include "stack.h"
#include "stack_reu.h"
#include "point.h"
#include "rotations.h"
#if __has_include("input.h")
  #include "input.h"          // user’s private input (gitignored)
#else
  #include "input.example.h"  // checked-in sample input
#endif

// structure for the distance fingerprint between beacons i and j
struct Distance {
    uint24_t priority;
    uint8_t i;
    uint8_t j;
};
// distance fingerprints for each scanner stored on the REU
typedef StackREU<Distance, uint16_t, n_beacon_pairs> Fingerprint;
Fingerprint _fingerprints[n_scanners];
Point _scanner_pos[n_scanners];
// complication due to uneven number of beacons per scanner
uint16_t _fp_lengths[n_scanners];
uint8_t _num_beacons[n_scanners];
uint8_t t = 0;


void get_distance_fingerprints() {
    // used for sortin the distance hashes
    static Heap<Distance, uint16_t, n_beacon_pairs> dist_hashes;
    dist_hashes.clear();
    // iterate over scanners and store the distance hashes in the REU
    uint24_t addr = 0;
    constexpr uint16_t fp_size = 2048;
    for (uint8_t s = 0; s < n_scanners; ++s) {
        tick(t++ & (uint8_t)7);
        _fingerprints[s].init(addr);
        addr += fp_size;
        dist_hashes.clear();
        const Point* const& points = scanners[s];
        uint8_t i;
        // iterate over pairs of beacons
        for (i = 0; i < n_beacons; ++i) {
            if (points[i].empty()) break;
            uint16_t dd[3];
            for (uint8_t j = i + 1; j < n_beacons; ++j) {
                // take the 24-bit Pearson hash of the sored partial L1 distances
                dd[0] = ABS(points[i].x - points[j].x);
                dd[1] = ABS(points[i].y - points[j].y);
                dd[2] = ABS(points[i].z - points[j].z);
                if (dd[0] > dd[1])
                    SWAP(dd[0], dd[1]);
                if (dd[1] > dd[2])
                    SWAP(dd[1], dd[2]);
                if (dd[0] > dd[1])
                    SWAP(dd[0], dd[1]);
                dist_hashes.push({.priority = hash24((uint8_t *)dd, sizeof(dd)), .i = i, .j = j});
            }
        }
        _num_beacons[s] = i;
        // store the sorted distance fingerprints on the REU
        // (and calculate the total number of distances per scanner)
        while (dist_hashes.size()) {
            _fingerprints[s].push(dist_hashes.pop());
            ++_fp_lengths[s];
        }
    }
}


// for the mapping it is enough to identify a single pair of beacon pairs
struct Mapping {
    int8_t s1;              // scanner 1 index
    int8_t s2;              // scanner 1 index
    int8_t idx1[12];        // beacon IDs for scanner 1
    int8_t idx2[12];        // beacon IDs for scanner 2
    int8_t matched_pair[4]; // IDs for pair of beacons of identical distance fingerprint
    // create a new mapping with the roles of the two scanners swapped
    Mapping swap() const {
        Mapping n;
        n.s1 = s2; n.s2 = s1;
        memcpy(n.idx1, &idx2[0], 12); memcpy(n.idx2, &idx1[0], 12);
        n.matched_pair[0] = matched_pair[2]; n.matched_pair[1] = matched_pair[3];
        n.matched_pair[2] = matched_pair[0]; n.matched_pair[3] = matched_pair[1];
        return n;
    }
};
// global variable to hold all the identified mappings
Mapping _mappings[50];
int8_t _n_mappings = 0;

void find_matches() {
    for (uint8_t s1 = 0; s1 < n_scanners; ++s1) {
        tick(t++ & (uint8_t)7);
        for (uint8_t s2 = s1 + 1; s2 < n_scanners; ++s2) {
            Mapping& m = _mappings[_n_mappings];
            const Fingerprint& f1 = _fingerprints[s1];
            const Fingerprint& f2 = _fingerprints[s2];
            Distance dd1, dd2;
            // find matching distance fingerprints in O(n) scan (possible due to sorting)
            const uint16_t len1 = _fp_lengths[s1];
            const uint16_t len2 = _fp_lengths[s2];
            uint8_t point_pair_found = 0;
            uint16_t i = 0, j = 0;
            uint8_t idx1[n_beacons] = {};
            uint8_t idx2[n_beacons] = {};
            while (i < len1 && j < len2) {
                f1.getInto(i, &dd1); f2.getInto(j, &dd2);
                if (dd1.priority == dd2.priority) {
                    idx1[dd1.i] = 1; idx2[dd2.i] = 1;
                    idx1[dd1.j] = 1; idx2[dd2.j] = 1;
                    if (!point_pair_found) {
                        m.matched_pair[0] = dd1.i; m.matched_pair[1] = dd1.j;
                        m.matched_pair[2] = dd2.i; m.matched_pair[3] = dd2.j;
                        point_pair_found = 1;
                    }
                    ++i; ++j;
                }
                else if (dd1.priority < dd2.priority) ++i;
                else ++j;
            }
            uint8_t l1 = 0, l2 = 0;
            for (uint8_t k = 0; k < n_beacons; ++k) {
                if (idx1[k]) m.idx1[l1++] = k;
                if (idx2[k]) m.idx2[l2++] = k;
            }
            if (l1 == 12) {
                m.s1 = s1; m.s2 = s2;
                ++_n_mappings;
            }
        }
    }
};


void find_transformation(const Mapping& m, uint8_t& rotation_idx, Point& translation) {
    const Point& p_A1 = scanners[m.s1][m.matched_pair[0]];
    const Point p_B[2] = {scanners[m.s2][m.matched_pair[2]], scanners[m.s2][m.matched_pair[3]]};
    uint8_t j = 0;
    while (m.idx1[j] == m.matched_pair[0] || m.idx1[j] == m.matched_pair[1]) ++j;
    const Point& original = scanners[m.s1][m.idx1[j]];
    for (uint8_t i = 0; i < n_rotations; ++i) {
        const Matrix& rot = rotation_matrices[i];
        const Point trans_o[2] = {subtract_points(p_A1, rotate_point(p_B[0], rot)),
                                  subtract_points(p_A1, rotate_point(p_B[1], rot))};
        for (uint8_t p = 0; p < 2; ++p) {
            const Point& trans = trans_o[p];
            for (uint8_t k = 0; k < 12; ++k) {
                if (transform_point(scanners[m.s2][m.idx2[k]], rot, trans) == original) {
                    rotation_idx = i;
                    translation = trans;
                    return;
                }
            }
        }
    }
}


void locate_scanners() {
    uint8_t located[n_scanners] = {};
    located[0] = 1;
    uint8_t n_located = 1;
    _scanner_pos[0] = {0, 0, 0};
    while (n_located < n_scanners) {
        tick(t++ & (uint8_t)7);
        for (uint8_t i = 0; i < _n_mappings; ++i) {
            const Mapping& m = _mappings[i];
            if (!(located[m.s1] ^ located[m.s2]))
                continue;
            Mapping n = !located[m.s1] ? m.swap() : m;
            uint8_t rotation_idx;
            Point translation;
            find_transformation(n, rotation_idx, translation);
            // back-transform the points of the located scanner
            for (uint8_t j = 0; j < _num_beacons[n.s2]; ++j) {
                scanners[n.s2][j] = transform_point(
                    scanners[n.s2][j],
                    rotation_matrices[rotation_idx],
                    translation
                );
            }
            located[n.s2] = 1;
            _scanner_pos[n.s2] = translation;
            ++n_located;
        }
    }
}


uint16_t count_points() {
    HashSet<Point, uint16_t, 1024> all_points;
    all_points.clear();
    for (uint8_t s = 0; s < n_scanners; ++s) {
        for (uint8_t b = 0; b < _num_beacons[s]; ++b) {
            all_points.insert(scanners[s][b]);
        }
    }
    return all_points.size();
}


uint16_t max_scanner_distance() {
    uint16_t distance = 0;
    for (uint8_t i = 0; i < n_scanners; ++i) {
        const Point& p1 = _scanner_pos[i];
        for (uint8_t j = i + 1; j < n_scanners; ++j) {
            const Point& p2 = _scanner_pos[j];
            const uint16_t d = ABS(p1.x - p2.x) + ABS(p1.y - p2.y) + ABS(p1.z - p2.z);
            if (d > distance)
                distance = d;
        }
    }
    return distance;
}


int main(void) {
    init(19);

    get_distance_fingerprints();
    find_matches();
    locate_scanners();

    printf("part 1: %u\n", count_points());
    printf("part 2: %u\n", max_scanner_distance());

    finish();
}