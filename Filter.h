//
// Created by R on 2/02/2016.
//

#ifndef TRACER_FILTER_H
#define TRACER_FILTER_H

#include "Vector.h"

class Tracer;

class Filter {
public:
    virtual Vec3 samplePixel(int x, int y, int width, int height, int s, Tracer *tracer) = 0;

protected:
    //Generates s * s 2-D samples between the range l to u
    void stratifiedSample(int s, float *samples, float l, float u);
};

class BoxFilter : public Filter {

public:
    Vec3 samplePixel(int x, int y, int width, int height, int s, Tracer *tracer);
};

#endif //TRACER_FILTER_H
