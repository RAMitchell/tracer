//
// Created by R on 2/02/2016.
//

#include "Vector.h"
#include "Tracer.h"

namespace Filter {
    void stratifiedSample(int s, float *samples, float l, float u) {
        float stride = (u - l) / s;
        for (int y = 0; y < s; y++) {
            for (int x = 0; x < s; x++) {
                samples[(s * y + x) * 2] = Random(l + stride * x, l + stride * (x + 1));
                samples[(s * y + x) * 2 + 1] = Random(l + stride * y, l + stride * (y + 1));
            }
        }
    }

    void stratifiedDiskSample(int s, float *samples, float radius) {
        stratifiedSample(s, samples, 0, 1);

        for (int i = 0; i < s * s; i++) {
            float r = sqrt(samples[2 * i]) * radius;
            float theta = samples[2 * i + 1] * 2 * M_PI;

            samples[2 * i] = r * cosf(theta);
            samples[2 * i + 1] = r * sinf(theta);
        }
    }

    Vec3 evaluateSamples(int x, int y, int width, int height, int s, const Tracer *tracer, float *samples) {

        //Generate lens samples for depth of field
        float lensSamples[s * s * 2];
        stratifiedDiskSample(s, lensSamples, 1.0);

        //Must shuffle samples so they don't correlate with camera samples
        //Cast to doubles so we shuffle them in pairs
        std::random_shuffle((double *) lensSamples, (double *) lensSamples + s * s);

        Vec3 pixel;
        float pixelWidth = 2.0f / width;
        float pixelHeight = 2.0f / height;

        for (int i = 0; i < s * s; i++) {
            //Find pixel centre in -1 to 1 coordinates
            float x_f = (((float) x + samples[i * 2] + pixelWidth / 2.0f) / float(width)) * 2 - 1;
            float y_f = (((float) y + samples[i * 2 + 1] + pixelHeight / 2.0f) / float(height)) * 2 - 1;
            Ray r = tracer->getCameraRay(x_f, y_f, lensSamples[i * 2], lensSamples[i * 2 + 1]);
            pixel += tracer->trace(r);
        }
        return pixel / (s * s);
    }

    Vec3 box(int x, int y, int width, int height, int s, const Tracer *tracer) {
        float samples[s * s * 2];

        stratifiedSample(s, samples, -0.5, 0.5);
        return evaluateSamples(x, y, width, height, s, tracer, samples);

    }

    Vec3 tent(int x, int y, int width, int height, int s, const Tracer *tracer) {
        float samples[s * s * 2];

        stratifiedSample(s, samples, -1.0, 1.0);

        //Inversion sampling
        for (float &f: samples) {
            if (f > 0) {
                f = 1 - sqrt(1 - f);
            }
            else if (f < 0) {
                f = -(1 - sqrt(1 - fabs(f)));
            }
        }

        return evaluateSamples(x, y, width, height, s, tracer, samples);

    }

}