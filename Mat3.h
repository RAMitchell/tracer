//
// Created by R on 26/02/2016.
//

#ifndef TRACER_MAT3_H
#define TRACER_MAT3_H


#include "Vector.h"

class Mat3 {
public:
    float mat[9] = {0};
    Mat3() {
        mat[0] = 1;
        mat[4] = 1;
        mat[8] = 1;
    }

    Mat3(float a, float b, float c, float d, float e, float f, float g, float h, float i) {
        mat[0] = a;
        mat[1] = b;
        mat[2] = c;
        mat[3] = d;
        mat[4] = e;
        mat[5] = f;
        mat[6] = g;
        mat[7] = h;
        mat[8] = i;
    }

    Mat3 &scale(Vec3 s) {
        mat[0] *= s.x;
        mat[4] *= s.y;
        mat[8] *= s.z;
        return *this;
    }

    Mat3 &transpose() {
        float tmp[9] = {mat[0], mat[3], mat[6], mat[1], mat[4], mat[7], mat[2], mat[5], mat[8]};
        std::copy(tmp, tmp + 9, mat);
        return *this;
    }

    //Radians
    Mat3 &rotate(Vec3 axis, float a);

    void operator*=(const Mat3 &other) {
        float tmp[9];
        tmp[0] = mat[0] * other.mat[0] + mat[1] * other.mat[3] + mat[2] * other.mat[6];
        tmp[1] = mat[0] * other.mat[1] + mat[1] * other.mat[4] + mat[2] * other.mat[7];
        tmp[2] = mat[0] * other.mat[2] + mat[1] * other.mat[5] + mat[2] * other.mat[8];
        tmp[3] = mat[3] * other.mat[0] + mat[4] * other.mat[3] + mat[5] * other.mat[6];
        tmp[4] = mat[3] * other.mat[1] + mat[4] * other.mat[4] + mat[5] * other.mat[7];
        tmp[5] = mat[3] * other.mat[2] + mat[4] * other.mat[5] + mat[5] * other.mat[8];
        tmp[6] = mat[6] * other.mat[0] + mat[7] * other.mat[3] + mat[8] * other.mat[6];
        tmp[7] = mat[6] * other.mat[1] + mat[7] * other.mat[4] + mat[8] * other.mat[7];
        tmp[8] = mat[6] * other.mat[2] + mat[7] * other.mat[5] + mat[8] * other.mat[8];

        std::copy(tmp, tmp + 9, mat);

    }

};

static Mat3 operator*(Mat3 m1, Mat3 m2) {
    m1 *= m2;
    return m1;

}

static Vec3 operator*(Mat3 m, Vec3 v) {

    return Vec3(v.Dot(Vec3(m.mat[0], m.mat[1], m.mat[2])),
                v.Dot(Vec3(m.mat[3], m.mat[4], m.mat[5])), v.Dot(Vec3(m.mat[6], m.mat[7], m.mat[8])));
}

#endif //TRACER_MAT3_H
