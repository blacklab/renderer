#include "../lib/clipping.h"
#include "../lib/output.h"
#include "helper.h"
#include <catch.hpp>

TEST_CASE("Clip polygon at a plane", "[clipping]") {
    auto tri = std::vector<Point3f>{{0, 0, 0}, {1, 0, 0}, {1, 1, 0}};
    auto res = clip_polygon_at_plane(tri, {1, 0, 0}, 0);
    REQUIRE(res == tri);

    tri = std::vector<Point3f>{{0, 0, 0}, {1, 0, 0}, {1, 1, 0}};
    res = clip_polygon_at_plane(tri, {1, 0, 0}, 0.5f);
    auto expected = std::vector<Point3f>{
        {0.5f, 0.5f, 0}, {0.5f, 0, 0}, {1, 0, 0}, {1, 1, 0}};
    REQUIRE(res == expected);

    tri = std::vector<Point3f>{{0, 1, 0}, {0, 0, 0}, {1, 0, 0}};
    Normal3f normal = normalize(Normal3f{-1, 1, 0});
    res = clip_polygon_at_plane(tri, normal, 0);
    expected = std::vector<Point3f>{{0.5f, 0.5f, 0}, {0, 1, 0}, {0, 0, 0}};
    REQUIRE(res == expected);
}

TEST_CASE("Simple line clipping test", "[clipping]") {
    Bbox3f box{{-1, -1, -1}, {1, 1, 1}};

    Point3f p0 = {0, 0, -10};
    Point3f p1 = {0, 0, 10};

    REQUIRE(clip_line_aabb(p0, p1, box));
    REQUIRE(p0 == (Point3f{0, 0, -1}));
    REQUIRE(p1 == (Point3f{0, 0, 1}));

    p0 = {0, -10, 0};
    p1 = {0, 10, 0};

    REQUIRE(clip_line_aabb(p0, p1, box));
    REQUIRE(p0 == (Point3f{0, -1, 0}));
    REQUIRE(p1 == (Point3f{0, 1, 0}));

    p0 = {-10, 0, 0};
    p1 = {10, 0, 0};

    REQUIRE(clip_line_aabb(p0, p1, box));
    REQUIRE(p0 == (Point3f{-1, 0, 0}));
    REQUIRE(p1 == (Point3f{1, 0, 0}));

    p0 = {-10, 10, 0};
    p1 = {10, 10, 0};

    REQUIRE(!clip_line_aabb(p0, p1, box));
    REQUIRE(p0 == (Point3f{-10, 10, 0}));
    REQUIRE(p1 == (Point3f{10, 10, 0}));
}

TEST_CASE("Big triangle clipping at aabb", "[clipping]") {
    Bbox3f box{{-1, -1, -1}, {1, 1, 1}};
    auto tri = test_triangle({0, 0, -10}, {10, 0, 10}, {-10, 0, 10});
    auto res = clip_triangle_at_aabb(tri, box);
    REQUIRE(res == (Bbox3f{{-1, 0, -1}, {1, 0, 1}}));
    REQUIRE(res.planar(Axis3::Y));
}

TEST_CASE("Peak triangle clipping at aabb", "[clipping]") {
    Bbox3f box{{0, 0, 0}, {2, 2, 2}};
    auto tri = test_triangle({-1, -1, 0}, {1, 1, 0}, {1, -1, 0});
    auto res = clip_triangle_at_aabb(tri, box);
    REQUIRE(res == (Bbox3f{{0, 0, 0}, {1, 1, 0}}));
    REQUIRE(res.planar(Axis3::Z));
}

TEST_CASE("Simple triangle clipping at aabb", "[clipping]") {
    Bbox3f box{{-1, -1, -1}, {1, 1, 1}};

    auto tri = test_triangle({-1, -1, -1}, {2, 2, 2}, {2, -1, 2});
    auto res = clip_triangle_at_aabb(tri, box);
    REQUIRE(res == box);

    tri = test_triangle({0, 0, 0}, {0.5, 0.5, 0.5}, {0.5, 0, 0.5});
    res = clip_triangle_at_aabb(tri, box);
    REQUIRE(res == (Bbox3f{{0, 0, 0}, {0.5, 0.5, 0.5}}));
}

TEST_CASE("Random triangle clipping at aabb", "[clipping]") {
    Bbox3f box{{-1, -1, -1}, {1, 1, 1}};
    for (int i = 0; i < 10000; ++i) {
        auto tri = random_triangle();
        if (!intersect_triangle_box(tri, box)) {
            continue;
        }

        auto res = clip_triangle_at_aabb(tri, box);

        REQUIRE(-EPS < res.p_min.x - box.p_min.x);
        REQUIRE(-EPS < res.p_min.y - box.p_min.y);
        REQUIRE(-EPS < res.p_min.z - box.p_min.z);

        REQUIRE(-EPS < box.p_max.x - res.p_max.x);
        REQUIRE(-EPS < box.p_max.y - res.p_max.y);
        REQUIRE(-EPS < box.p_max.z - res.p_max.z);
    }
}
