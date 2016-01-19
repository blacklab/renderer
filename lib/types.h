#pragma once

#include <assimp/types.h>
#include <assert.h>
#include <vector>


using Vec = aiVector3D;


Vec operator/(int a, const Vec& v) {
    return {
        static_cast<float>(a)/v.x,
        static_cast<float>(a)/v.y,
        static_cast<float>(a)/v.z
    };
}


// AABB
struct Box {
    Vec min, max;
};


struct Ray : public aiRay {
    Ray(const Vec& pos, const Vec& dir)
        : aiRay(pos, dir)
        , invdir(1/dir)
    {}

    Ray(const aiRay& ray) : Ray(ray.pos, ray.dir) {}

    // pos, dir are in aiRay
    Vec invdir;
};

struct Triangle {
    const Vec vertices[3];
    const Vec normals[3];
    const aiColor4D diffuse;
};
using Triangles = std::vector<Triangle>;