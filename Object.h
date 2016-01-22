#pragma once

#include "Includes.h"
#include "Vector.h"
#include "Camera.h"
#include "Random.h"
#include "Material.h"

#define NO_HIT        (1e5f)

struct Object;

struct Hit {
    Vec3 p, n;
    float d;
    const Object *obj;

    bool IsValid() const {
        return d < NO_HIT - EPS;
    }

    Hit() : d(NO_HIT) { }

    Hit(Vec3 p, Vec3 n, float d, const Object *obj) :
            p(p), n(Normalize(n)), d(d), obj(obj) { }
};


struct Object {

    Vec3 Position;
    float Radius;

    Material *material;

    Object(Vec3 p, float r, Material *m) : Position(p), Radius(r), material(m) { }

    Hit Intersect(Ray ray) const {
        Vec3 op = Position - ray.o;
        float b = op.Dot(ray.d);
        float det = b * b - op.Dot(op) + Radius * Radius;

        if (det >= 0) {
            det = sqrtf(det);
            Vec3 h;
            if (b - det > 0) {
                h = ray * (b - det);
                return {h, h - Position, b - det, this};
            }
            if (b + det > 0) {
                h = ray * (b + det);
                return {h, h - Position, b + det, this};
            }
        }

        return Hit();
    }

    Vec3 GetPointInside() const {
        return RandomSphere(Radius) + Position;
    }

};