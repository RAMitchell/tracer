//
// Created by R on 3/02/2016.
//

#include <iostream>
#include "PostProcess.h"
#include "Includes.h"

Vec3 rgbToYxy(Vec3 v) {
    Vec3 XYZ;
    XYZ.x = v.Dot(Vec3(0.4124, 0.3576, 0.1805));
    XYZ.y = v.Dot(Vec3(0.2126, 0.7152, 0.0722));
    XYZ.z = v.Dot(Vec3(0.0193, 0.1192, 0.9505));


    return Vec3(XYZ.y, XYZ.x / (XYZ.x + XYZ.y + XYZ.z), XYZ.y / (XYZ.x + XYZ.y + XYZ.z));
}

Vec3 YxyTorgb(Vec3 v) {
    Vec3 XYZ(v.y * (v.x / v.z), v.x, (1.0 - v.y - v.z) * (v.x / v.z));

    return Vec3(XYZ.Dot(Vec3(3.2405, -1.5371, -0.4985)), XYZ.Dot(Vec3(-0.9693, 1.8760, 0.0416)),
                XYZ.Dot(Vec3(0.0556, -0.2040, 1.0572)));
}

Vec3 gammaCorrect(Vec3 v) {
    v.x = powf(v.x, 0.45f);
    v.y = powf(v.y, 0.45f);
    v.z = powf(v.z, 0.45f);

    return v;
}

void PostProcess::linearTM(const std::vector<Vec3> &backBuffer, std::vector<Vec3> &displayBuffer) {
    for (int i = 0; i < backBuffer.size(); i++) {
        displayBuffer[i] = gammaCorrect(backBuffer[i]);
    }
}

inline float log_add(float a, float b) {
    return log(a) + log(1 + b / a);
}

inline float log_sub(float a, float b) {
    return log(a) + log(1 - b / a);
}

void PostProcess::bloom(const std::vector<Vec3> &backBuffer, std::vector<Vec3> &displayBuffer, int height, int width) {
    //Kernel radius not including current pixel
    int k_rad = 5;

    //Calculate Lavg and Lmax
    double delta = 0.0001;

    //Transform RGB to Yxy and calculate average log luminance
    double Lavg = 0.0;
    double Lmax = 0.0;
    for (int i = 0; i < backBuffer.size(); i++) {
        Vec3 Yxy = rgbToYxy(backBuffer[i]);
        displayBuffer[i] = Yxy;
        Lavg += log(delta + Yxy.x);
        Lmax = std::max((float) Lmax, Yxy.x);
    }

    Lavg = exp(Lavg / backBuffer.size());

    float threshold = Lavg;
}

void PostProcess::ReinhardTM(const std::vector<Vec3> &backBuffer, std::vector<Vec3> &displayBuffer) {

    double delta = 0.0001;

    //Transform RGB to Yxy and calculate average log luminance
    double Lavg = 0.0;
    double Lmax = 0.0;
    for (int i = 0; i < backBuffer.size(); i++) {
        Vec3 Yxy = rgbToYxy(backBuffer[i]);
        displayBuffer[i] = Yxy;
        Lavg += log(delta + Yxy.x);
        Lmax = std::max((float) Lmax, Yxy.x);
    }

    Lavg = exp(Lavg / backBuffer.size());


    double middleGrey = 1.03 - (2.0 / (2 + log10(Lavg + 1.0)));

    float scalar = middleGrey / Lavg;

    for (int i = 0; i < backBuffer.size(); i++) {
        float Lscaled = displayBuffer[i].x * scalar;

        //Apply reinhard operator
        displayBuffer[i].x = (Lscaled * (1 + (Lscaled / Lmax))) / (1.0f + Lscaled);

        //Convert back to RGB
        displayBuffer[i] = gammaCorrect(YxyTorgb(displayBuffer[i]));

    }


}
