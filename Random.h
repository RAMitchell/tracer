#pragma once
#include "Vector.h"
#include <cmath>

#define M_PI 3.14159265359

static unsigned long xorshf96(void) {
	static unsigned long x=123456789, y=362436069, z=521288629;
	unsigned long t;
	x ^= x << 16;
	x ^= x >> 5;
	x ^= x << 1;

	t = x;
	x = y;
	y = z;
	z = t ^ x ^ y;

	return z;
}


static float Random(float l, float u) {
	auto r = xorshf96() % 1000000;
	return l + (u - l) * float(r) * 1e-6;
}

static Vec3 RandomDir() {
	float theta = Random(0, 2 * M_PI);
	float phi = acos(Random(-1, 1));
	float z = cos(phi);
	float u = sqrt(1 - z * z);
	float x = u * cos(theta);
	float y = u * sin(theta);
	return Vec3(x, y, z);
}

static Vec3 RandomSphere(float r) {
	return RandomDir() * Random(0, r);
}

static Vec3 RandomHemi(Vec3 n) {
	Vec3 d = RandomDir();
	return (d.Dot(n) > 0 ? d : -d);
}