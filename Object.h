#pragma once

#include "Includes.h"
#include "Vector.h"
#include "Camera.h"
#include "Random.h"
#include "Hit.h"


class Material;

class Hit;

struct Object {

    Vec3 Position;
    float Radius;

    Material *material;

    Object(Vec3 p, float r, Material *m) : Position(p), Radius(r), material(m) { }

    Hit Intersect(const Ray &ray) const {
        Vec3 op = Position - ray.o;
        float b = op.Dot(ray.d);
        if (b < 0.0f) {
            return Hit();
        }
        float det = b * b - op.Dot(op) + Radius * Radius;

        if (det >= 0) {
            det = sqrtf(det);
            Vec3 h;
            if (b - det > 0) {
                h = ray.o + ray.d * (b - det);
                return {h, h - Position, b - det, -ray.d, ray.type, this, ray.depth};
            }
            if (b + det > 0) {
                h = ray.o + ray.d * (b + det);
                return {h, h - Position, b + det, -ray.d, ray.type, this, ray.depth};
            }
        }

        return Hit();
    }

    Vec3 GetPointInside() const {
        return RandomSphere(Radius) + Position;
    }

    //Find the maximum distance of the surface from its position. Used for generating bounding boxes
    float getMaxRadius()const{
        return Radius;
    }

};