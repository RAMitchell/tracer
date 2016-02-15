//
// Created by R on 4/02/2016.
//
#include "Material.h"
#include "Object.h"
#include "Scene.h"
#include "Tracer.h"

float Material::fresnel(const Hit &h, const float &f0) {

    //Fresnel term - schlick's approximation
    float f_ = clamp(1.0 - h.normal.Dot(h.wo), 0.0, 1.0);

    return f0 + (1 - f0) * f_ * f_ * f_ * f_ * f_;
}

Vec3  Material::cosine_sample(const Vec3 &n, float exp) {

    float eps1 = Random(0, 1);
    float theta = Random(0, 2 * M_PI);

    Vec3 w = n;
    Vec3 u = Vec3(0.0034, 1.0, 0.0071).Cross(w);
    u.Normalize();
    Vec3 v = w.Cross(u);

    float sin_alpha = sqrt(1.0 - pow(eps1, 2.0 / (exp + 1)));
    float x = sin_alpha * cos(theta);
    float y = sin_alpha * sin(theta);
    float z = pow(eps1, 1 / (exp + 1));

    return (u * x + v * y + w * z).Normalize();
}

Vec3 Material::direct(const Hit &h, const Scene *scene) {

    //Accumulate direct lighting
    Vec3 direct;
    for (Object *light : scene->Lights()) {

        //Do not check light against itself
        if (light == h.obj) {
            continue;
        }


        //Create random direction towards sphere
        Vec3 sw = light->Position - h.position;
        Vec3 su = Vec3(0.0034, 1.0, 0.0071).Cross(sw).Normalize();
        Vec3 sv = sw.Cross(su);

        float cos_a_max = sqrt(
                1.0 -
                clamp(((light->Radius * light->Radius) / ((sw).Dot(sw))), 0.0f, 1.0f));

        float eps1 = Random(0, 1);
        float cos_a = 1 - eps1 + eps1 * cos_a_max;
        float sin_a = sqrt(1 - cos_a * cos_a);
        float phi = Random(0, 2 * M_PI);
        Vec3 wi = (su * cos(phi) * sin_a + sv * sin(phi) * sin_a + sw * cos_a).Normalize();

        float NoL = h.normal.Dot(wi);

        //Light source is behind surface
        if (NoL <= 0) {
            continue;
        }

        //Test shadow Ray
        if (scene->IsVisible(h.position + h.normal * EPS, wi, light)) {

            //Solid angle
            float omega = 2 * M_PI * (1.0 - cos_a_max);


            direct += light->material->emmissive *
                      light->material->colour * omega * f(h.wo, wi, h.normal) * NoL;

        }
    }
    return direct;
}

Vec3 Lambert::shade(const Hit &h, const Tracer *tracer) {

    //Direct lighting
    Vec3 L = direct(h, tracer->getScene());

    //Emmissive
    if (h.rType == CAMERA) {
        L += h.obj->material->getEmmissive() * h.obj->material->getColour();
    }

    float pdf;
    Vec3 wi;
    Vec3 fr = sample_f(h, wi, pdf);
    float ndotwi = std::max(0.0f, h.normal.Dot(wi));

    L += fr * tracer->Trace({h.position + wi * EPS, wi, DIFFUSE, h.depth + 1}) * ndotwi / pdf;

    return L;
}

Vec3  Lambert::f(const Vec3 &wo, const Vec3 &wi, const Vec3 &n) {
    return colour * M_1_PI;

}


Vec3  Lambert::sample_f(const Hit &h, Vec3 &wi, float &pdf) {
    wi = cosine_sample(h.normal);
    pdf = h.normal.Dot(wi);
    //Make sure pdf is greater than zero
    if (pdf <= 0.0) {
        pdf = EPS;
    }

    return colour;
}

Vec3 Phong::shade(const Hit &h, const Tracer *tracer) {

    //Direct lighting
    Vec3 L = direct(h, tracer->getScene());

    //Emmissive
    if (h.rType == CAMERA) {
        L += h.obj->material->getEmmissive() * h.obj->material->getColour();
    }

    float pdf;
    Vec3 wi;
    Vec3 fr = sample_f(h, wi, pdf);
    float ndotwi = std::max(0.0f, h.normal.Dot(wi));

    L += fr * tracer->Trace({h.position + wi * EPS, wi, GLOSSY, h.depth + 1}) * ndotwi / pdf;

    return L;
}

Vec3 Phong::f(const Vec3 &wo, const Vec3 &wi, const Vec3 &n) {

    Vec3 r = (-wi).Reflect(n);
    float rdotwo = r.Dot(wo);
    float normalisation = (exp + 2.0f) / (2.0f * M_PI);

    return std::max(0.0, pow(rdotwo, exp) * normalisation);
}

Vec3 Phong::sample_f(const Hit &h, Vec3 &wi, float &pdf) {

    Vec3 r = (-h.wo).Reflect(h.normal);
    wi = cosine_sample(r, exp);

    //Make sure pdf is greater than zero
    pdf = std::max(EPS,h.normal.Dot(wi));

    return 1.0;

}

Vec3 FresnelBlend::shade(const Hit &h, const Tracer *tracer) {
    float f = fresnel(h, f0);
    float eps = Random(0, 1);
    if (eps < f) {
        return specular.shade(h, tracer);
    }
    else {
        return diffuse.shade(h, tracer);
    }

}

Vec3 FresnelBlend::f(const Vec3 &wo, const Vec3 &wi, const Vec3 &n) {
    return 0;
}

Vec3 FresnelBlend::sample_f(const Hit &h, Vec3 &wi, float &pdf) {
    return 0;
}
