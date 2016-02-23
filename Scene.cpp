#include "Scene.h"
#include "Material.h"


void Scene::addSphere(Sphere s) {
    spheres.push_back(s);
}



Hit Scene::bvhIntersect(const Ray &r) const {

    return bvh.intersect(r);

}

Hit Scene::intersect(const Ray &r) const {
    Hit hit;

    for (const Object* obj : objects) {
        Hit h = obj->intersect(r);
        if (h.distance < hit.distance) {
            hit = h;
        }
    }

    return hit;
}

void Scene::init() {

    for( Sphere &s: spheres) {
        objects.push_back(&s);

        if (s.material()->isEmissive()) {
            lights.push_back(&s);
        }
    }

    buildBvh();
}


bool Scene::isVisible(Vec3 o, Vec3 d, const Object *test) const {

    Ray ray = {o, d, SHADOW, -1};

    Hit h = bvh.intersect(ray);

    if (h.obj != test) {
        return false;
    }
    return true;
}