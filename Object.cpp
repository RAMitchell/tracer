//
// Created by R on 20/02/2016.
//

#include "Object.h"
#include "Material.h"
#include "Scene.h"

Vec3    Sphere::albedo(float u, float v) const {
    return mat->albedo(u, v);
}

void getSphereUV(const Vec3&d,float &u, float &v) {
    float theta = acos(d.y);
    float phi = atan2(d.z, d.x);
    u = (M_PI + phi) / (2 * M_PI);
    v = theta * M_1_PI;
}

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
            Vec3 wo = (h - p).Normalize();
            getSphereUV(wo, u, v);
            return {h, wo, -ray.d, u, v, ray.type, b - det, this, ray.depth};
        }
        if (b + det > 0) {

            h = ray.o + ray.d * (b + det);
            float u, v;
            Vec3 wo = (h - p).Normalize();
            getSphereUV(wo, u, v);
            return {h, wo, -ray.d, u, v, ray.type, b + det, this, ray.depth};
        }
    }

    return Hit();
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
               omega * h.obj->material()->f(h, wi) * NoL;

    }
    return Vec3();
}


Hit Triangle::intersect(const Ray &ray) const {
    
    Vec3 e1 = mesh->getVertex(p2) - mesh->getVertex(p1);
    Vec3 e2 = mesh->getVertex(p3) - mesh->getVertex(p1);
    Vec3 s1 = ray.d.Cross(e2);
    float divisor = s1.Dot(e1);

    if (divisor == 0.0f) {
        return Hit();
    }

    float invDivisor = 1.0f / divisor;

    //First barycentric coordinate
    Vec3 d = ray.o - mesh->getVertex(p1);
    float b1 = d.Dot(s1) * invDivisor;
    if (b1 < 0.0f || b1 > 1.0f) {
        return Hit();
    }

    //Second coordinate
    Vec3 s2 = d.Cross(e1);
    float b2 = ray.d.Dot(s2) * invDivisor;
    if (b2 < 0.0f || b1 + b2 > 1.0f) {
        return Hit();
    }

    //Compute t
    float t = e2.Dot(s2) * invDivisor;

    if (t < 0.0f) {
        return Hit();
    }

    Vec3 h = ray.o + ray.d * t;
    float b0 = 1-b1-b2;
    Vec3 n = Normalize(b0 * mesh->getNormal(n1) + b1 * mesh->getNormal(n2) + b2 * mesh->getNormal(n3));

    return Hit(h, n, -ray.d, 0, 0, ray.type, t, this, ray.depth);
}

BBox Triangle::getBB() const {

    float x0 = std::min(mesh->getVertex(p1).x, std::min(mesh->getVertex(p2).x, mesh->getVertex(p3).x));
    float x1 = std::max(mesh->getVertex(p1).x, std::max(mesh->getVertex(p2).x, mesh->getVertex(p3).x));
    float y0 = std::min(mesh->getVertex(p1).y, std::min(mesh->getVertex(p2).y, mesh->getVertex(p3).y));
    float y1 = std::max(mesh->getVertex(p1).y, std::max(mesh->getVertex(p2).y, mesh->getVertex(p3).y));
    float z0 = std::min(mesh->getVertex(p1).z, std::min(mesh->getVertex(p2).z, mesh->getVertex(p3).z));
    float z1 = std::max(mesh->getVertex(p1).z, std::max(mesh->getVertex(p2).z, mesh->getVertex(p3).z));

    //Add Epsilon so that bounding box never becomes a flat plane
    return {x0, x1 + EPS, y0, y1 + EPS, z0, z1 + EPS};
}


Vec3  Triangle::directLighting(const Hit &h, const Scene *scene) const {

    return Vec3();

}
Vec3    Triangle::albedo(float u, float v) const {
    return mesh->getMaterial()->albedo(u, v);
}
