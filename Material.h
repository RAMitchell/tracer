//
// Created by R on 18/01/2016.
//

#ifndef TRACER_MATERIAL_H
#define TRACER_MATERIAL_H

#include "Vector.h"

#define M_1_PI 0.31830988618

class Material {

protected:
    //Generates random cosine weighted unit vector around normal
    Vec3 cosine_sample(Vec3 n) {

        float theta = Random(0.0, 2.0 * M_PI);
        float r = Random(0.0, 1.0);
        float rs = sqrt(r);

        Vec3 w = n;
        Vec3 u = (fabs(w.x) > 0.1 ? Vec3(0, 1, 0) : Vec3(1)).Cross(w);
        u.Normalize();
        Vec3 v = w.Cross(u);

        return (u * cos(theta) * rs + v * sin(theta) * rs + w * sqrt(1.0 - r)).Normalize();
    }

public:
    virtual Vec3 bounce(Vec3 d, Vec3 n, float &weight, float fresnel) = 0;

    virtual float brdf(Vec3 d, Vec3 n, Vec3 l, float fresnel) = 0;

    Vec3 colour;
    float emmissive;
};

//Modified phong reflectance model according to "Using the modified phong reflectance model for physically based rendering", Eric P. Lafortune, Yves D. Willems
class MPhong : public Material {

    float exp;
public:
    MPhong(Vec3 c, float e, float exp_) {
        emmissive = e;
        colour = c;
        exp = exp_;
    }

    Vec3 bounce(Vec3 d, Vec3 n, float &weight, float fresnel) {
        float r1 = Random(0, 1);
        //Specular bounce
        if (r1 < fresnel) {

            float eps1 = Random(0, 1);
            float theta = Random(0, 1);

            Vec3 w = n;
            Vec3 u = (fabs(w.x) > 0.1 ? Vec3(0, 1, 0) : Vec3(1)).Cross(w);
            u.Normalize();
            Vec3 v = w.Cross(u);

            float sin_alpha = sqrt(1.0 - pow(eps1, 2.0 / (exp + 1)));
            float x = sin_alpha * cos(theta);
            float y = sin_alpha * sin(theta);
            float z = pow(eps1, 1 / (exp + 1));
            Vec3 new_dir = (u * x + v * y + w * z).Normalize();

            float NoL = n.Dot(new_dir);

            weight = ((exp + 2.0) / (exp + 1.0)) * clamp(n.Dot(new_dir), 0.0f, 1.0f);
            return new_dir;

        }
        else {
            weight = 1.0;
            return cosine_sample(n);
        }
    }

    float brdf(Vec3 d, Vec3 n, Vec3 l, float fresnel) {
        Vec3 r = (-l).Reflect(n).Normalize();
        float diffuse = (1.0 - fresnel) * M_1_PI;
        float normalisation = (exp + 2.0) / (2.0 * M_PI);
        float specular = fresnel * pow((clamp(r.Dot(d), 0.0f, 1.0f)), exp) * normalisation;

        return diffuse + specular;
    }
};

class Lambert : public Material {
public:

    Lambert(Vec3 c, float e) {
        emmissive = e;
        colour = c;
    }


    //Cosign weighted importance sampling
    Vec3 bounce(Vec3 d, Vec3 n, float &weight, float fresnel) {

        weight = 1.0;
        return cosine_sample(n);

    }

    float brdf(Vec3 d, Vec3 n, Vec3 l, float fresnel) {

        return M_1_PI;
    }
};

#endif //TRACER_MATERIAL_H
