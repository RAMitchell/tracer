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


    Vec3 Yxy;
    Yxy.x = XYZ.y;
    float denom = (XYZ.x + XYZ.y + XYZ.z);
    if (denom == 0.0f) {
        Yxy.y = 0.0f;
        Yxy.z = 0.0f;
    }
    else {
        Yxy.y = XYZ.x / denom;
        Yxy.z = XYZ.y / denom;
    }


    return Yxy;
}

Vec3 YxyTorgb(Vec3 v) {
    Vec3 XYZ;//(v.y * (v.x / v.z), v.x, (1.0 - v.y - v.z) * (v.x / v.z));
    XYZ.y = v.x;
    if (v.z == 0) {
        XYZ.x = 0.0f;
        XYZ.z = 0.0f;
    }
    else {
        float a = v.x / v.z;
        XYZ.x = v.y * a;
        XYZ.z = (1.0 - v.y - v.z) * a;
    }


    return Vec3(XYZ.Dot(Vec3(3.2405, -1.5371, -0.4985)), XYZ.Dot(Vec3(-0.9693, 1.8760, 0.0416)),
                XYZ.Dot(Vec3(0.0556, -0.2040, 1.0572)));
}

Vec3 gammaCorrect(Vec3 v) {
    Vec3 a = v;
    v.x = powf(std::max(v.x,0.0f), 0.45f);
    v.y = powf(std::max(v.y,0.0f), 0.45f);
    v.z = powf(std::max(v.z,0.0f), 0.45f);

    return v;
}

void PostProcess::linearTM(const std::vector<Vec3> &backBuffer, std::vector<Vec3> &displayBuffer) {
    for (int i = 0; i < backBuffer.size(); i++) {
        displayBuffer[i] = gammaCorrect(backBuffer[i]);
    }
}


void boxBlurH(const std::vector<Vec3> &in, std::vector<Vec3> &out, int k_r, int height, int width) {

    for (int i = 0; i < height; i++) {
        int ti = i * width;
        int li = ti - k_r;
        int ri = ti + k_r + 1;
        Vec3 val = in[ti] * (k_r + 1);
        for (int j = 1; j < k_r + 1; j++) {
            val += in[ti + std::min(j, width-1)];
        }

        out[ti] = val / (k_r + k_r + 1);
        for (int j = 1; j < width; j++) {
            val -= in[std::max(li++, ti)];
            val += in[std::min(ri++, ti + width - 1)];
            out[ti + j] = val / (k_r + k_r + 1);

        }
    }
}


void boxBlurV(const std::vector<Vec3> &in, std::vector<Vec3> &out, int k_r, int height, int width) {

    for (int j = 0; j < width; j++) {
        int ti = j;
        int li = ti - (k_r * width);
        int ri = ti + ((k_r + 1) * width);
        Vec3 val = in[ti] * (k_r + 1);
        for (int i = 1; i < k_r + 1; i++) {
            val += in[ti + std::min(i,height - 1) * width];
        }

        out[ti] = val / (k_r + k_r + 1);
        for (int i = 1; i < width; i++) {
            val -= in[std::max(li,ti)];
            li+=width;
            val += in[std::min(ri, ti +  (height - 1) * width)];
            ri  += width;
            out[ti + i *width] = val / (k_r + k_r + 1);

        }
    }
}

void boxesForGauss(std::vector<int> &boxes, float sigma, int n_boxes) {
    float wIdeal = sqrt((12 * sigma * sigma / n_boxes) + 1);
    int wl = wIdeal;
    if (wl % 2 == 0) {
        wl--;
    }
    int wu = wl + 2;

    float mIdeal = (12 * sigma * sigma - n_boxes * wl * wl - 4 * n_boxes * wl - 3 * n_boxes) / (-4 * wl - 4);
    int m = round(mIdeal);
    for (int i = 0; i < n_boxes; i++) {
        boxes.push_back(i < m ? wl : wu);
    }
}

void boxBlur(std::vector<Vec3> &in, std::vector<Vec3> &out, int k_r, int height, int width) {
    out = in;
    boxBlurH(out, in, k_r, height, width);
    boxBlurV(in, out, k_r, height, width);


}

void  PostProcess::gaussianBlur(std::vector<Vec3> &in, std::vector<Vec3> &out, int k_r, int height, int width) {

    double start = clock();
    int n_boxes = 3;
    std::vector<int> boxes;
    boxesForGauss(boxes, k_r, n_boxes);

    boxBlur(in, out, (boxes[0] - 1) / 2, height, width);
    boxBlur(out, in, (boxes[1] - 1) / 2, height, width);
    boxBlur(in, out, (boxes[2] - 1) / 2, height, width);

}

void PostProcess::bloom(const std::vector<Vec3> &backBuffer, std::vector<Vec3> &displayBuffer,
                        std::vector<Vec3> &bloomBuffer, int k_r, int height, int width, float amount) {

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

    float threshold = Lavg;//  + ((Lmax - Lavg) * 0.5 );

    for (int i = 0; i < backBuffer.size(); i++) {
        if (displayBuffer[i].x > threshold) {
            displayBuffer[i] = backBuffer[i] * (( displayBuffer[i].x - Lavg)/(Lmax - Lavg));
        }
        else {
            displayBuffer[i] = Vec3();
        }
    }

    gaussianBlur(displayBuffer, bloomBuffer, k_r, height, width);
    for (int i = 0; i < backBuffer.size(); i++) {
        bloomBuffer[i] = bloomBuffer[i] * amount + backBuffer[i];
    }
}

void PostProcess::ReinhardTM(const std::vector<Vec3> &in, std::vector<Vec3> &out) {


    double delta = 0.0001;

    //Transform RGB to Yxy and calculate average log luminance
    double Lavg = 0.0;
    double Lmax = 0.0;
    for (int i = 0; i < in.size(); i++) {
        Vec3 Yxy = rgbToYxy(in[i]);
        out[i] = Yxy;

        Lavg += log(delta + Yxy.x);
        Lmax = std::max((float) Lmax, Yxy.x);
    }

    Lavg = exp(Lavg / in.size());

    double middleGrey = 1.03 - (2.0 / (2 + log10(Lavg + 1.0)));

    float scalar = middleGrey / Lavg;

    for (int i = 0; i < in.size(); i++) {
        float Lscaled = out[i].x * scalar;

        //Apply reinhard operator
        out[i].x = (Lscaled * (1 + (Lscaled / Lmax))) / (1.0f + Lscaled);

        //Convert back to RGB
        out[i] = gammaCorrect(YxyTorgb(out[i]));


    }


}
