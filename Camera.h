#pragma once
#include "Vector.h"

struct Ray {
	Vec3 o, d;
	Ray(Vec3 o, Vec3 d) : o(o), d(Normalize(d)) {}
	Vec3 operator*(float t) {
		return o + d * t;
	}
};

class Camera {
	Vec3 pos, dir, side, up;
	float aspect, fov;
public:
	Camera(Vec3 p = 0, Vec3 t = 0, Vec3 u = 0) : pos(p), aspect(1.0f) {
		Vec3 fd = t - p;
		Vec3 rd = fd.Cross(u);
		Vec3 ud = rd.Cross(fd);
		
		dir = Normalize(fd);
		side = Normalize(rd);
		up = Normalize(ud);
	}
	
	void SetAspect(float w, float h) {
		aspect = w / h;
	}
	
	void SetFOV(float f) {
		fov = atan(f);
	}
	
	Ray GetRay(float x, float y) {	
		Vec3 d = dir + side * x * fov * aspect + up * y * fov;
		return { pos, Normalize(d) };
	}
};