#pragma once

#include "Includes.h"
#include "Vector.h"
#include "Camera.h"
#include "Random.h"
#include "Hit.h"
#include "BVH.h"
#include "Mesh.h"


class Material;

class Hit;

class Scene;

class Object {


public:
    Object() { }

    virtual Hit intersect(const Ray &ray) const = 0;

    virtual BBox getBB() const = 0;

    virtual Vec3 directLighting(const Hit &h, const Scene *scene) const = 0;

    //Return centre of the object. Can be approximate.
    virtual Vec3 centre() const = 0;

    virtual Vec3 albedo(float u, float v) const = 0;

    virtual const Material *material() const = 0;


};

class Sphere : public Object {
private:
    const Material *mat;
    float radius;
    const Vec3 p;

public:
    Sphere(Vec3 p, float r) : radius(r), p(p) { }

    Hit intersect(const Ray &ray) const;

    BBox getBB() const;

    Vec3 directLighting(const Hit &h, const Scene *scene) const;

    Vec3 centre() const {
        return p;
    }

    Vec3 albedo(float u, float v) const;

    const Material *material() const {
        return mat;
    }

    void setMaterial( Material*m){
        mat = m;
    }


};

class Triangle : public Object {

public:
    Mesh *mesh;
    int p1, p2, p3;
    int n1, n2, n3;

    Triangle(Mesh *mesh, int p1, int p2, int p3, int n1, int n2, int n3) : mesh(mesh), p1(p1),
                                                                                      p2(p2), p3(p3), n1(n1), n2(n2),
                                                                                      n3(n3) { }


    Hit intersect(const Ray &ray) const;

    BBox getBB() const;

    Vec3 directLighting(const Hit &h, const Scene *scene) const;

    Vec3 centre() const {
        return (mesh->getVertex(p1) + mesh->getVertex(p2) + mesh->getVertex(p3)) / 3;
    }

    Vec3 albedo(float u, float v) const;

    const Material *material() const {
        return mesh->getMaterial();
    }

};
