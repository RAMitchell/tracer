#include "Scene.h"


void Scene::addSphere(Sphere s,std::string materialName) {
    s.setMaterial(getMaterial(materialName));
    spheres.push_back(s);
}

void Scene::addMesh( Mesh*m,std::string materialName){
    meshes.push_back(std::unique_ptr<Mesh>(m->clone()));
    meshes.back()->setMaterial(getMaterial(materialName));
}

Hit Scene::bvhIntersect(const Ray &r) const {

    return bvh.intersect(r);

}

Hit Scene::basicIntersect(const Ray &r) const {
    Hit hit;

    for (const Object *obj : objects) {
        Hit h = obj->intersect(r);
        if (h.distance < hit.distance) {
            hit = h;
        }
    }

    return hit;
}

void Scene::init() {

    for (Sphere &s: spheres) {
        objects.push_back(&s);

        if (s.material()->isEmissive()) {
            lights.push_back(&s);
        }
    }

    for( auto &m: meshes) {
        for (Triangle &t: m->getTriangles()) {
            objects.push_back(&t);
            if (t.material()->isEmissive()) {
                lights.push_back(&t);
            }
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