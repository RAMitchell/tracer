#pragma once

#include <memory>
#include "Includes.h"
#include "Camera.h"
#include "Scene.h"

struct RGB {
    uint8_t r, g, b;

    void operator=(const Vec3 &v) {
        r = uint8_t(clamp(v.x, 0.0f, 1.0f) * 255);
        g = uint8_t(clamp(v.y, 0.0f, 1.0f) * 255);
        b = uint8_t(clamp(v.z, 0.0f, 1.0f) * 255);
    }
};

class Tracer {
    int width, height, samples;
    int s;
    int  frame;
    std::vector<Vec3> buffer;
    std::vector<Vec3> backbuffer;

    Camera camera;
    Scene scene;

    Vec3 Trace(Ray ray, int depth = 5);

public:
    Tracer() : width(0), height(0), samples(0) { }

    Tracer(int w, int h, int s_) : width(w), height(h), samples(s_), frame(0) {

        buffer.resize(width * height);
        backbuffer.resize(width * height);

        s = int(sqrt(samples));


    }

    void Add(Vec3 obj, float r, Material *m) {
        scene.Add(obj, r, m);
    }

    void SetCamera(Vec3 p, Vec3 l, Vec3 u) {
        camera = Camera(p, l, u);
        camera.SetAspect(width, height);
        camera.SetFOV(M_PI / 13);
    }

    void Init() {

        scene.FindLights();
    }

    void Render();

    const Vec3 *GetImage() const { return buffer.data(); }

    int Width() const { return width; }

    int Height() const { return height; }
};