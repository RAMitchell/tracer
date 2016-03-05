#pragma once

#include "Vector.h"
#include "Random.h"

enum RType {
    CAMERA,
    GLOSSY,
    REFLECT,
    DIFFUSE,
    REFRACT,
    SHADOW
};

struct Ray {
    RType type;
    Vec3 o, d, d_inv;
    int depth;

    Ray(Vec3 o, Vec3 d, RType r_type, int depth) : o(o), d(Normalize(d)), type(r_type), depth(depth) {
        d_inv = Vec3(1 / d.x, 1 / d.y, 1 / d.z);
    }


    Vec3 operator*(float t) {
        return o + d * t;
    }
};

class Camera {
    Vec3 pos, dir, side, up;
    float aspect, fov, aperture, focalLength;

public:
    Camera(Vec3 p = 0, Vec3 t = 0, Vec3 u = 0) : pos(p), aspect(1.0f), aperture(0.0f), focalLength(1.0f) {
        Vec3 fd = t - p;
        Vec3 rd = fd.Cross(u);
        Vec3 ud = rd.Cross(fd);

        dir = Normalize(fd);
        side = Normalize(rd);
        up = Normalize(ud);
    }


    void SetFOV(float f) {
        fov = atan(f);
    }

    void setAperture(float radius, float fLength) {
        aperture = radius;
        focalLength = fLength;
    }

    Ray getRay(float x, float y, int x_max, int y_max, float lens_x, float lens_y) const {
        float aspect = x_max/y_max;
        Vec3 newPos = pos + side * (lens_x *aperture)  + up * (lens_y * aperture);
        Vec3 d = (dir + side * x * fov * aspect + up * y * fov).Normalize();
        Vec3 newT = pos + d * focalLength;
        return {newPos, Normalize(newT - newPos), CAMERA, 0};
    }
};