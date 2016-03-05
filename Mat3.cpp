//
// Created by R on 26/02/2016.
//

#include "Mat3.h"

Mat3 &Mat3::rotate(Vec3 axis, float a) {

    //Create ortho normal basis
    Vec3 w = axis.Normalize();
    Vec3 u = Vec3(0.0034, 1.0, 0.0071).Cross(w).Normalize();
    Vec3 v = w.Cross(u);


    Mat3 m1(u.x, v.x, w.x, u.y, v.y, w.y, u.z, v.z, w.z);
    Mat3 m2(cos(a), -sin(a), 0, sin(a), cos(a), 0, 0, 0, 1);
    Mat3 m3 = m1;
    m3.transpose();

    *this *= (m1 * m2 * m3);

    return *this;
}
