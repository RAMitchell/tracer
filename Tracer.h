#pragma once

#include <memory>
#include "Includes.h"
#include "Camera.h"
#include "Scene.h"
#include "Filter.h"
#include "PostProcess.h"


struct RGB {
    uint8_t r, g, b;

    void operator=(const Vec3 &v) {
        r = uint8_t(clamp(v.x, 0.0f, 1.0f) * 255);
        g = uint8_t(clamp(v.y, 0.0f, 1.0f) * 255);
        b = uint8_t(clamp(v.z, 0.0f, 1.0f) * 255);
    }
};


struct Tile {
    int xmin, xmax, ymin, ymax;

    Tile(int xmin_, int xmax_, int ymin_, int ymax_) : xmin(xmin_), xmax(xmax_), ymin(ymin_), ymax(ymax_) { }
};

class Tracer {
    int width, height, samples, bounces;
    int s;
    int tileSize;
    int frame;
    std::vector<Tile> tiles;
    std::vector<Vec3> displayBuffer;
    std::vector<Vec3> backBuffer;
    std::vector<Vec3> bloomBuffer;

    Camera camera;
    Scene scene;
    std::unique_ptr<Filter> filter;


public:
    Tracer() : width(0), height(0), samples(0), bounces(0) { }

    Tracer(int w, int h, int s_, int b, int ts) : width(w), height(h), samples(s_), bounces(b), frame(0), tileSize(ts) {

        displayBuffer.resize(width * height);
        backBuffer.resize(width * height);
        bloomBuffer.resize(width * height);

        s = int(sqrt(samples));

        filter = std::unique_ptr<Filter>(new BoxFilter());


    }

    Vec3 Trace(Ray ray) const;

    void Add(Vec3 obj, float r, Material *m) {
        scene.Add(obj, r, m);
    }

    void SetCamera(Vec3 p, Vec3 l, Vec3 u, float fov) {
        camera = Camera(p, l, u);
        camera.SetAspect(width, height);
        camera.SetFOV(fov);
    }

    void setEnvMap(std::string texture_filename, envProjection proj) {
        scene.setEnvMap(texture_filename, proj);
    }

    Camera &getCamera() {
        return camera;
    }

    void Init() {

        scene.FindLights();
        scene.buildBvh();
    }

    void Render();

    const Vec3 *GetImage() const { return displayBuffer.data(); }

    int getSamplesPP() {
        return s * s * (frame - 1);
    }

    const Scene *getScene() const {
        return &scene;
    }

    int Width() const { return width; }

    int Height() const { return height; }

private:
    void buildTiles();

    void renderTile(Tile &t);

    void postProcess();
};