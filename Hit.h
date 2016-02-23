//
// Created by R on 28/01/2016.
//

#ifndef TRACER_HIT_H
#define TRACER_HIT_H

#include "Vector.h"

#define NO_HIT        (1e5f)

struct Object;

struct Hit {
    Vec3 position, normal, wo;
    float u, v;
    RType rType;

    float distance;
    const Object *obj;

    int depth;

    Hit(const Vec3 &position, const Vec3 &normal, const Vec3 &wo, float u, float v, const RType &rType, float distance,
        const Object *obj, int depth) : position(position), normal(Normalize(normal)), wo(wo), u(u), v(v), rType(rType),
                                        distance(distance), obj(obj), depth(depth) { }

    bool IsValid() const {
        return distance < NO_HIT - EPS;
    }

    bool addEmissive() const {
        switch (rType) {
            case CAMERA:
                return true;
            case REFLECT:
                return true;
            case REFRACT:
                return true;
            default:
                return false;
        }
    }

    Hit() : distance(NO_HIT) { }

};

#endif //TRACER_HIT_H
