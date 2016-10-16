#include "../lib/triangle.h"
#include "../lib/types.h"

#include <math.h>
#include <random>

template <typename T>
bool is_within_tolerance(T value, T expected, T tolerance) {
    return expected - tolerance <= value && value <= expected + tolerance;
}

// Construct a triangle with trivial normals and colors.
Triangle test_triangle(Vec a, Vec b, Vec c) { return Triangle({a, b, c}); }

Vec random_vec() {
    static std::default_random_engine gen(0);
    static std::uniform_real_distribution<float> rnd(-10.f, 10.f);
    return {rnd(gen), rnd(gen), rnd(gen)};
};

// Construct a random vector lying on the unit sphere in the plane ax = pos.
Vec random_vec_on_unit_sphere(Axis ax, float pos) {
    static std::default_random_engine gen(0);
    static std::uniform_real_distribution<float> rnd(0, 2 * M_PI);

    auto phi = rnd(gen);
    auto x = cos(phi);
    auto y = sin(phi);

    Vec v;
    size_t ax_num = static_cast<size_t>(ax);
    v[AXES[ax_num]] = pos;
    v[AXES[(ax_num + 1) % 3]] = x;
    v[AXES[(ax_num + 2) % 3]] = y;
    return v;
}

Triangle random_triangle() {
    return test_triangle(random_vec(), random_vec(), random_vec());
}

// Construct a random triangle with vertices lying on the unit sphere in the
// plane ax = pos.
Triangle random_triangle_on_unit_sphere(Axis ax, float pos) {
    return test_triangle(random_vec_on_unit_sphere(ax, pos),
                         random_vec_on_unit_sphere(ax, pos),
                         random_vec_on_unit_sphere(ax, pos));
}

// Construct a random regular triangle with vertices lying on the unit sphere
// in the plane ax = pos. In particular, the 0-point lies in the triangle.
Triangle random_regular_triangle_on_unit_sphere(Axis ax, float pos) {
    static std::default_random_engine gen(0);
    static std::uniform_real_distribution<float> rnd(0, 2 * M_PI);

    auto phi = rnd(gen);
    Vec vertices[3];
    for (size_t i = 0; i < 3; ++i) {
        auto x = cos(phi + 2.f * M_PI / 3 * i);
        auto y = sin(phi + 2.f * M_PI / 3 * i);

        size_t ax_num = static_cast<size_t>(ax);
        vertices[i][AXES[ax_num]] = pos;
        vertices[i][AXES[(ax_num + 1) % 3]] = x;
        vertices[i][AXES[(ax_num + 2) % 3]] = y;
    }

    return test_triangle(vertices[0], vertices[1], vertices[2]);
}

// We need this operator only for tests.
bool operator==(const Box& b1, const Box& b2) {
    return b1.min == b2.min && b2.max == b2.max;
}

// We need this operator only for tests.
bool operator==(const Triangle& tria, const Triangle& trib) {
    return tria.vertices[0] == trib.vertices[0] &&
           tria.vertices[1] == trib.vertices[1] &&
           tria.vertices[2] == trib.vertices[2];
}
