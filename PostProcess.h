//
// Created by R on 3/02/2016.
//

#pragma once

#include <vector>
#include "Vector.h"

namespace PostProcess {

    void bloom(const std::vector<Vec3> &backBuffer, std::vector<Vec3> &displayBuffer, std::vector<Vec3> &bloomBuffer,
               int k_r, int height, int width,float amount);

    void linearTM(const std::vector<Vec3> &backBuffer, std::vector<Vec3> &displayBuffer);

    void ReinhardTM(const std::vector<Vec3> &in, std::vector<Vec3> &out);

    void  gaussianBlur(std::vector<Vec3> &in, std::vector<Vec3> &out, int k_r, int height,
                                    int width);
}
