//
// Created by R on 2/02/2016.
//

#ifndef TRACER_FILTER_H
#define TRACER_FILTER_H

#include "Vector.h"

class Tracer;

namespace Filter {

    Vec3 box(int x, int y, int width, int height, int s, const Tracer *tracer);

    Vec3 tent(int x, int y, int width, int height, int s, const Tracer *tracer);
}
#endif //TRACER_FILTER_H
