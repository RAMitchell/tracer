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

#define EPS		(1e-2f)


template <class T>
T clamp(T x, T min, T max) {
	return (x > max ? max : x < min ? min : x);
}


template <class T>
T lerp(T a, T b, float i) {
	return a * (1 - i) + b * i;
}


static float fresnel(float t, float f0) {
	return f0 + (1 - f0) * t * t * t * t * t;
}