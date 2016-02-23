#pragma once

#include "Includes.h"
#include "Object.h"
#include "Vector.h"
#include "Camera.h"
#include "BVH.h"
#include "EnvMap.h"


class Scene {
    BVH bvh;
    std::vector< Sphere> spheres;
    std::vector<const Object *> objects;

    std::vector<const Object *> lights;
    EnvMap envMap;

public:

    void addSphere(Sphere s);

    void setEnvMap(Vec3 c){
        envMap = EnvMap(c);
    }
    void setEnvMap(std::string texture_filename,envProjection proj) {
        envMap = EnvMap(texture_filename,proj);
    }

    Vec3 env(Vec3 d) const {
        return envMap.sample(d);
    }

    void buildBvh() {
        bvh.build(objects);
    };

    void init();

    Hit intersect(const Ray &ray) const;

    Hit bvhIntersect(const Ray &ray) const;

    bool isVisible(Vec3 start, Vec3 end, const Object *obj) const;

    const std::vector<const Object *> &getLights() const { return lights; }
};