#pragma once

#include "types.h"
#include <assimp/types.h>

//
// Return -1 if there is no intersection, otherwise return r s.t. the
// intersection point is P0 + r * (P1 - P0).
//
// If n is normalized, then r is exactly the euklidean distance from the ray to
// the plane.
//
// Args:
//   P0, P1: Points defining a ray from P0 to P1
//   V0, n: Plane through V0 with normal n
//
// Cf. http://geomalgorithms.com/a06-_intersect-2.html
//
float ray_plane_intersection(const Ray& ray, const Vec& V0, const Vec& n) {
    auto denom = n * ray.dir;
    if (denom == 0) {
        return -1;
    }

    auto nom = n * (V0 - ray.pos);
    auto r = nom / denom;

    return r < 0 ? -1 : r;
}


//
// Return -1 if there is no intersection, otherwise return r s.t. the
// intersection point is P0 + r * (P1 - P0).
//
// If n is normalized, then r is exactly the euklidean distance from the ray to
// the triangle.
//
// Args:
//   V0, V1, V2: Points defining a triangle
//
// Cf. http://geomalgorithms.com/a06-_intersect-2.html
//
bool ray_triangle_intersection(
    const Ray& ray, const Vec& V0, const Vec& V1, const Vec& V2,
    float& r, float& s, float& t)
{
    auto u = V1 - V0;
    auto v = V2 - V0;
    auto n = u^v;  // normal vector of the triangle

    r = ray_plane_intersection(ray, V0, n);
    if (r == -1) {
        return false;
    }

    auto P_int = ray.pos + r * ray.dir;
    auto w = P_int - V0;

    // precompute scalar products
    auto uv = u*v;
    auto wv = w*v;
    auto vv = v*v;
    auto wu = w*u;
    auto uu = u*u;

    auto denom = uv * uv - uu * vv;
    s = (uv * wv - vv * wu) / denom;

    if (s < 0) {
        return false;
    }

    t = (uv * wu - uu * wv) / denom;

    if (t < 0 || s + t > 1) {
        return false;
    }

    return true;
}

//
// Intersect a ray with an AABB (axis-aligned bounding box).
//
// Cf. http://people.csail.mit.edu/amy/papers/box-jgt.pdf
//
bool ray_box_intersection(const Ray& r, const Box& box) {
    float tx1 = (box.min.x - r.pos.x) * r.invdir.x;
    float tx2 = (box.max.x - r.pos.x) * r.invdir.x;

    float tmin = std::fmin(tx1, tx2);
    float tmax = std::fmax(tx1, tx2);

    float ty1 = (box.min.y - r.pos.y) * r.invdir.y;
    float ty2 = (box.max.y - r.pos.y) * r.invdir.y;

    tmin = std::fmax(tmin, std::fmin(ty1, ty2));
    tmax = std::fmin(tmax, std::fmax(ty1, ty2));

    float tz1 = (box.min.z - r.pos.z) * r.invdir.z;
    float tz2 = (box.max.z - r.pos.z) * r.invdir.z;

    tmin = std::fmax(tmin, std::fmin(tz1, tz2));
    tmax = std::fmin(tmax, std::fmax(tz1, tz2));

    return !(tmax < tmin);
}
