//
// Created by R on 3/02/2016.
//

#pragma once

#include <vector>
#include "Vector.h"

namespace PostProcess {

    void bloom(const std::vector<Vec3> &backBuffer, std::vector<Vec3> &displayBuffer, int height, int width);

    void linearTM(const std::vector<Vec3> &backBuffer, std::vector<Vec3> &displayBuffer);

    void ReinhardTM(const std::vector<Vec3> &backBuffer, std::vector<Vec3> &displayBuffer);
}
/*
class Tonemapper {

public:
    virtual void process(std::vector<Vec3> &backBuffer, std::vector<Vec3> &displayBuffer) =0;

protected:

};

class LinearTM : public Tonemapper {

public:

    void process(std::vector<Vec3> &backBuffer, std::vector<Vec3> &displayBuffer);
};

class ReinhardTM : public Tonemapper {

public:

    void process(std::vector<Vec3> &backBuffer, std::vector<Vec3> &displayBuffer);
};
*/
