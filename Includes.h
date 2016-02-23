#pragma once

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <algorithm>
#include <cstring>
#include <array>
#include <cmath>
#include <vector>
#include <random>
#include <ctime>
#include <cstdint>
#include <iostream>

#define EPS        (1e-3f)
#define M_PI 3.14159265359
#define M_1_PI 0.31830988618

template<class T>
T clamp(T x, T min, T max) {
    return (x > max ? max : x < min ? min : x);
}


template<class T>
T lerp(T a, T b, float i) {
    return a * (1 - i) + b * i;
}


struct Timer {
    size_t start;
    std::string description;

    Timer() { }

    Timer(std::string d) : start(clock()) {
        description = d;
    }

    void print() {
        std::cout << description << " time: " << ((double) clock() - start) / CLOCKS_PER_SEC << "\n";
    }

    void reset() {
        start = clock();
    }
};
