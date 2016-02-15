//
// Created by R on 2/02/2016.
//

#include "Vector.h"
#include "Tracer.h"

void  Filter::stratifiedSample(int s, float *samples, float l, float u) {
    float stride = (u - l) / s;
    for (int y = 0; y < s; y++) {
        for (int x = 0; x < s; x++) {
            samples[(s * y + x) * 2] = Random(l + stride * x, l + stride * (x + 1));
            samples[(s * y + x) * 2 + 1] = Random(l + stride * y, l + stride * (y + 1));
        }
    }
}

Vec3  BoxFilter::samplePixel(int x, int y, int width, int height, int s, Tracer *tracer) {
    Vec3 pixel;
    float samples[s * s * 2];

    stratifiedSample(s, samples,  - 0.5, 0.5);

    float pixelWidth = 2.0f / width;
    float pixelHeight = 2.0f / height;

    for (int i = 0; i < s * s; i++) {
        //Find pixel centre in -1 to 1 coordinates
        float x_f = (((float) x + samples[i * 2] + pixelWidth / 2.0f) / float(width)) * 2 - 1;
        float y_f = (((float) y + samples[i * 2 + 1] + pixelHeight / 2.0f) / float(height)) * 2 - 1;
        pixel += tracer->Trace(tracer->getCamera().GetRay(x_f, y_f));
    }


    return pixel / (s * s);
}
