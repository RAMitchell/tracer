//
// Created by R on 18/01/2016.
//

#ifndef TRACER_MATERIAL_H
#define TRACER_MATERIAL_H

#include "Vector.h"
#include "Random.h"


class Hit;

class Tracer;

class Scene;

class Material {

protected:
    Vec3 colour = Vec3();
    float emmissive = 0.0;

    //Generates random cosine weighted unit vector around normal
    Vec3 cosine_sample(const Vec3 &n, float exp = 1);

    Vec3 direct(const Hit &h, const Scene *scene);

    float fresnel(const Hit &h, const float &f0);

public:
    virtual Vec3 shade(const Hit &h, const Tracer *tracer) = 0;

    Vec3 getColour() {

        return colour;
    }

    float getEmmissive() {
        return emmissive;
    }

private:
    virtual Vec3 f(const Vec3 &wo, const Vec3 &wi, const Vec3 &n) = 0;

    virtual Vec3 sample_f(const Hit &h, Vec3 &wi, float &pdf) = 0;


};

class Phong : public Material {

    float exp;
public:
    Phong(float exp) : exp(exp) {
    }

    Vec3 shade(const Hit &h, const Tracer *tracer);

    Vec3 f(const Vec3 &wo, const Vec3 &wi, const Vec3 &n);

    Vec3 sample_f(const Hit &h, Vec3 &wi, float &pdf);
};

class Lambert : public Material {
public:

    Lambert(Vec3 c, float e) {
        emmissive = e;
        colour = c;
    }

    Vec3 shade(const Hit &h, const Tracer *tracer);

    Vec3 f(const Vec3 &wo, const Vec3 &wi, const Vec3 &n);

    Vec3 sample_f(const Hit &h, Vec3 &wi, float &pdf);
};

class FresnelBlend : public Material {
private:
    Lambert diffuse;
    Phong specular;
    float f0;
public:
    FresnelBlend(Vec3 c, float e, float exp, float f0) : diffuse(c, e), specular(exp), f0(f0) {

    }

    Vec3 shade(const Hit &h, const Tracer *tracer);

    Vec3 f(const Vec3 &wo, const Vec3 &wi, const Vec3 &n);

    Vec3 sample_f(const Hit &h, Vec3 &wi, float &pdf);
};

class Refract : public Material {
private:
    float ior;
public:
    Refract(float ior) : ior(ior) {
    }

    Vec3 shade(const Hit &h, const Tracer *tracer);

    Vec3 f(const Vec3 &wo, const Vec3 &wi, const Vec3 &n);

    Vec3 sample_f(const Hit &h, Vec3 &wi, float &pdf);
};

class Mirror : public Material {
public:
    Vec3 shade(const Hit &h, const Tracer *tracer);

    Vec3 f(const Vec3 &wo, const Vec3 &wi, const Vec3 &n);

    Vec3 sample_f(const Hit &h, Vec3 &wi, float &pdf);

};

#endif //TRACER_MATERIAL_H
