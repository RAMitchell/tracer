#pragma once

#include "Includes.h"
#include "Object.h"
#include "Vector.h"
#include "Camera.h"
#include "BVH.h"
#include "EnvMap.h"


class Scene {
    BVH bvh;
    std::vector<Object> objects;

    std::vector<Object *> lights;
    EnvMap envMap;

public:

    void Add(Vec3 p, float r, Material *m);

    void setEnvMap(std::string texture_filename,envProjection proj) {
        envMap = EnvMap(texture_filename,proj);
    }

    Vec3 env(Vec3 d) const {
        return envMap.sample(d);
    }

    void buildBvh() {
        bvh.build(objects);
    };

    void FindLights();

    Hit Intersect(const Ray &ray) const;

    Hit bvhIntersect(const Ray &ray) const;

    bool IsVisible(Vec3 start, Vec3 end, Object *obj) const;

    const std::vector<Object *> &Lights() const { return lights; }
};