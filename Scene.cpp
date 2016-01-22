#include "Scene.h"


void Scene::Add(Vec3 p, float r, Material*m){
    objects.emplace_back(p, r, m);
}


void Scene::FindLights() {
    for (Object &obj : objects) {
        if (obj.material->emmissive > 0.0) {
            lights.push_back(&obj);
        }
    }
}


Hit Scene::Intersect(Ray ray) const {
    Hit hit;

    for (const Object &obj : objects) {
        Hit h = obj.Intersect(ray);
        if (h.d < hit.d) {
            hit = h;
        }
    }

    return hit;
}


bool Scene::IsVisible(Vec3 o, Vec3 d, Object *test) const {

    Ray ray = {o, d};
    float distance = (o-test->Position).Length()-test->Radius;
    for (const Object &obj : objects) {
        if(&obj == test){
            continue;
        }

        Hit h = obj.Intersect(ray);
        if (h.d <  distance ) {
            return false;
        }
    }
    return true;
}