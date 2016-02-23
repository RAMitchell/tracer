//
// Created by R on 20/02/2016.
//

#include "Object.h"
#include "Material.h"
#include "Scene.h"

Hit  Sphere::intersect(const Ray &ray) const {
    Vec3 op = p - ray.o;
    float b = op.Dot(ray.d);
    if (b < 0.0f) {
        return Hit();
    }
    float det = b * b - op.Dot(op) + radius * radius;

    if (det >= 0) {
        det = sqrtf(det);
        Vec3 h;
        if (b - det > 0) {
            h = ray.o + ray.d * (b - det);
            float u, v;
            getUV(h, u, v);
            Vec3 wo = (h - p).Normalize();
            return {h, wo, -ray.d, u, v, ray.type, b - det, this, ray.depth};
        }
        if (b + det > 0) {

            h = ray.o + ray.d * (b + det);
            float u, v;
            getUV(h, u, v);
            Vec3 wo = (h - p).Normalize();
            return {h, wo, -ray.d, u, v, ray.type, b + det, this, ray.depth};
        }
    }

    return Hit();
}

Vec3   Sphere::albedo(float u, float v) const {
    return mat->albedo(u, v);
}


BBox Sphere::getBB() const {
    return BBox(p.x - radius, p.x + radius, p.y - radius, p.y + radius, p.z - radius, p.z + radius);
}

Vec3  Sphere::directLighting(const Hit &h, const Scene *scene) const {

    //Create random direction towards sphere
    Vec3 sw = p - h.position;
    Vec3 su = Vec3(0.0034, 1.0, 0.0071).Cross(sw).Normalize();
    Vec3 sv = sw.Cross(su);

    float cos_a_max = sqrt(
            1.0 -
            clamp(((radius * radius) / ((sw).Dot(sw))), 0.0f, 1.0f));

    float eps1 = Random(0, 1);
    float cos_a = 1 - eps1 + eps1 * cos_a_max;
    float sin_a = sqrt(1 - cos_a * cos_a);
    float phi = Random(0, 2 * M_PI);
    Vec3 wi = (su * cos(phi) * sin_a + sv * sin(phi) * sin_a + sw * cos_a).Normalize();

    float NoL = h.normal.Dot(wi);

    //Light source is behind surface
    if (NoL <= 0) {
        return Vec3();
    }

    //Test shadow Ray
    if (scene->isVisible(h.position + h.normal * EPS, wi, this)) {

        //Solid angle
        float omega = 2 * M_PI * (1.0 - cos_a_max);


        return mat->getEmmissive() *
               omega * h.obj->material()->f(h,wi) * NoL;

    }
    return Vec3();
}

void Sphere::getUV(const Vec3 &h, float &u, float &v) const {
    Vec3 d = (h - p).Normalize();
    float theta = acos(d.y);
    float phi = atan2(d.z, d.x);
    u = (M_PI + phi) / (2 * M_PI);
    v = theta * M_1_PI;
}
