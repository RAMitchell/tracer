#pragma once

#include "Includes.h"
#include "Vector.h"
#include "Camera.h"
#include "Random.h"
#include "Hit.h"
#include "BVH.h"


class Material;

class Hit;

class Scene;

class Object {

protected:
    const Vec3 p;

    const Material *mat;

public:
    Object(Vec3 p, Material *m) : p(p), mat(m) { }

    virtual Hit intersect(const Ray &ray) const = 0;

    virtual Vec3 albedo(float u, float v) const = 0;

    virtual BBox getBB() const = 0;

    virtual Vec3 directLighting(const Hit& h, const Scene *scene)const = 0;

    Vec3 position() const {
        return p;
    }

    const Material *material() const {
        return mat;
    }

private:
    virtual void getUV(const Vec3 &h, float &u, float &v) const = 0;
};

class Sphere : public Object {
private:
    float radius;

public:
    Sphere(Vec3 p, float r, Material *m) : Object(p, m), radius(r) { }

    Hit intersect(const Ray &ray) const;

    Vec3 albedo(float u, float v) const;

    BBox getBB() const;

    Vec3 directLighting(const Hit& h, const Scene *scene)const;

private:

    void getUV(const Vec3 &h, float &u, float &v) const;

};