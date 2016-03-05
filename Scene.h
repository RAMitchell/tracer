#pragma once

#include "Includes.h"
#include "Object.h"
#include "Vector.h"
#include "Camera.h"
#include "BVH.h"
#include "EnvMap.h"
#include "Mesh.h"


class Scene {
    BVH bvh;
    std::vector<Sphere> spheres;
    //std::vector<Triangle> triangles;
    std::vector<std::unique_ptr<Mesh>> meshes;
    std::map<std::string, std::unique_ptr<Material>> materials;

    //Reference to every traceable object in the scene
    std::vector<const Object *> objects;
    //Reference to every  light emitting object in the scene
    std::vector<const Object *> lights;

    EnvMap envMap;
    Camera camera;

public:

    void addMaterial(std::string name, Material *mat) {
        materials[name] = std::unique_ptr<Material>(mat);
    }

    Material *getMaterial(std::string material) {
        if (materials.count(material) == 1) {
            return materials[material].get();
        }
        else {
            std::cerr << "Material '" << material << "' not found.\n";
            return NULL;
        }
    }

    void addSphere(Sphere s,std::string materialName);

    void addMesh( Mesh*m,std::string materialName);

    void setEnvMap(Vec3 c) {
        envMap = EnvMap(c);
    }

    void setEnvMap(std::string texture_filename, envProjection proj) {
        envMap = EnvMap(texture_filename, proj);
    }

    void setCamera(Vec3 p, Vec3 l, Vec3 u, float fov, float aperture = 0, float focalLength = 1) {
        camera = Camera(p, l, u);
        camera.SetFOV(fov);
        camera.setAperture(aperture, focalLength);
    }

    const Camera *getCamera() {
        return &camera;
    }

    Vec3 env(Vec3 d) const {
        return envMap.sample(d);
    }

    void buildBvh() {
        bvh.build(objects);
    };

    void init();

    Hit basicIntersect(const Ray &ray) const;

    Hit bvhIntersect(const Ray &ray) const;

    bool isVisible(Vec3 start, Vec3 end, const Object *obj) const;

    const std::vector<const Object *> &getLights() const { return lights; }
};