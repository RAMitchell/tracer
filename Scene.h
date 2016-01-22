#pragma once
#include "Includes.h"
#include "Object.h"
#include "Vector.h"
#include "Camera.h"


class Scene {
	std::vector<Object> objects;
	
	std::vector<Object*> lights;
public:

	void Add(Vec3 p, float r, Material *m);
	void FindLights();
	
	Hit Intersect(Ray ray) const;
	bool IsVisible(Vec3 start, Vec3 end, Object* obj) const;
	
	const std::vector<Object*>& Lights() const { return lights; }
};