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
    RType rType;

    float distance;
    const Object *obj;

    int depth;

    bool IsValid() const{
        return distance < NO_HIT - EPS;
    }
    bool addEmissive() const{
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

    Hit(Vec3 p, Vec3 n, float d, Vec3 wo_, RType rt, const Object *obj,int depth) :
            position(p), normal(Normalize(n)), distance(d), wo(wo_), rType(rt), obj(obj), depth(depth) { }
};

#endif //TRACER_HIT_H
