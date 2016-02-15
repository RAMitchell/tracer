#include "Scene.h"
#include "Material.h"


void Scene::Add(Vec3 p, float r, Material *m) {
    objects.emplace_back(p, r, m);
}


void Scene::FindLights() {
    for (Object &obj : objects) {
        if (obj.material->getEmmissive() > 0.0) {
            lights.push_back(&obj);
        }
    }
}

Hit Scene::bvhIntersect(const Ray &r) const {

    return  bvh.intersect(r);

}
Hit Scene::Intersect(const Ray &r) const {
    Hit hit;

    for (const Object &obj : objects) {
        Hit h = obj.Intersect(r);
        if (h.distance < hit.distance) {
            hit = h;
        }
    }

    return hit;
}


bool Scene::IsVisible(Vec3 o, Vec3 d, Object *test) const {

    Ray ray = {o, d, SHADOW, -1};
    float distance = (o - test->Position).Length() - test->Radius;
    for (const Object &obj : objects) {
        if (&obj == test) {
            continue;
        }

        Hit h = obj.Intersect(ray);
        if (h.distance < distance) {
            return false;
        }
    }
    return true;
}