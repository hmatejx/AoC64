#ifndef __POINT_H__
#define __POINT_H__

#include <stdint.h>
#include "../lib/pearson.h"
#include "rotations.h"

// Struct to represent a point in 3D space using smaller data types
struct Point {
    int16_t x, y, z;

    uint16_t hash() const { return hash16((uint8_t *)this, sizeof(Point)); }
    bool operator==(const Point& other) const { return x == other.x && y == other.y && z == other.z; }
    uint8_t empty() const { return x == 0 && y == 0 && z == 0; }
};

Point subtract_points(const Point& p1, const Point& p2) {
    return { .x = p1.x - p2.x, .y = p1.y - p2.y, .z = p1.z - p2.z };
}

Point add_points(const Point& p1, const Point& p2)  {
    return { .x = p1.x + p2.x, .y = p1.y + p2.y, .z = p1.z + p2.z };
}

#define DOT(r,c) (r)[0]*c.x + (r)[1]*c.y + (r)[2]*c.z

Point rotate_point(const Point& p, const Matrix& m) {
    return { .x = DOT(m[0], p), .y = DOT(m[1], p), .z = DOT(m[2], p) };
}

Point transform_point(const Point& from, const Matrix& r, const Point& t) {
    return add_points(rotate_point(from, r), t);
}


#endif // __POINT_H__