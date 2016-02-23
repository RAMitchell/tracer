//
// Created by R on 18/01/2016.
//

#ifndef TRACER_MATERIAL_H
#define TRACER_MATERIAL_H

#include "Vector.h"
#include "Random.h"
#include "Texture.h"


class Hit;

class Tracer;

class Scene;

class Material {

private:
    Vec3 colour;
    Vec3 emmissive;
    Texture *alb;


protected:
    Material() { }

    Material(const Vec3 &colour, const Vec3 &emmissive, Texture *alb) : colour(colour), emmissive(emmissive),
                                                                        alb(alb) { }

//Generates random cosine weighted unit vector around normal
    Vec3 cosine_sample(const Vec3 &n, float exp = 1) const;

    Vec3 direct(const Hit &h, const Scene *scene) const;

    float fresnel(const Hit &h, const float &f0) const;

public:
    virtual Vec3 shade(const Hit &h, const Tracer *tracer) const = 0;

    Vec3 albedo(float u, float v) const {

        if (alb != NULL) {
            return alb->sample(u, v);
        }
        return colour;
    }

    Vec3 getEmmissive() const {
        return emmissive;
    }

    bool isEmissive() const {
        return (emmissive.x > 0 || emmissive.y > 0 || emmissive.z > 0);
    }

    virtual Vec3 f(const Hit &h, const Vec3 &wi) const = 0;

    virtual Vec3 sample_f(const Hit &h, Vec3 &wi, float &pdf) const = 0;


};

class Phong : public Material {

    float exp;
public:
    Phong(float exp) : exp(exp), Material(Vec3(0.99), 0, NULL) {
    }

    Vec3 shade(const Hit &h, const Tracer *tracer) const;

    Vec3 f(const Hit &h, const Vec3 &wi) const;

    Vec3 sample_f(const Hit &h, Vec3 &wi, float &pdf) const;
};

class Lambert : public Material {
public:

    Lambert(Vec3 c) : Material(c, 0, NULL) {
    }

    Lambert(Texture *tex) : Material(0, 0, tex) {
    }

    Lambert(Vec3 c, Vec3 e) : Material(c, e, NULL) {
    }

    Vec3 shade(const Hit &h, const Tracer *tracer) const;

    Vec3 f(const Hit &h, const Vec3 &wi) const;

    Vec3 sample_f(const Hit &h, Vec3 &wi, float &pdf) const;
};

class FresnelBlend : public Material {
private:
    Lambert diffuse;
    Phong specular;
    float f0;
public:
    FresnelBlend(Vec3 c, float e, float exp, float f0) : diffuse(c, e), specular(exp), f0(f0), Material(c, 0, NULL) {

    }

    Vec3 shade(const Hit &h, const Tracer *tracer) const;

    Vec3 f(const Hit &h, const Vec3 &wi) const;

    Vec3 sample_f(const Hit &h, Vec3 &wi, float &pdf) const;
};

class Refract : public Material {
private:
    float ior;
public:
    Refract(float ior) : ior(ior), Material(Vec3(0.99), 0, NULL) {
    }

    Vec3 shade(const Hit &h, const Tracer *tracer) const;

    Vec3 f(const Hit &h, const Vec3 &wi) const;

    Vec3 sample_f(const Hit &h, Vec3 &wi, float &pdf) const;
};

class Mirror : public Material {
public:
    Mirror() : Material(Vec3(0.99), 0, NULL) { }

    Vec3 shade(const Hit &h, const Tracer *tracer) const;

    Vec3 f(const Hit &h, const Vec3 &wi) const;

    Vec3 sample_f(const Hit &h, Vec3 &wi, float &pdf) const;

};

#endif //TRACER_MATERIAL_H
