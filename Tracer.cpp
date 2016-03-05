#include <omp.h>
#include "Tracer.h"
#include "Material.h"


void Tracer::buildTiles() {
    for (int y = 0; y < height; y += tileSize) {
        for (int x = 0; x < width; x += tileSize) {
            tiles.push_back(Tile(x, clamp(x + tileSize, 0, width), y, clamp(y + tileSize, 0, height)));
        }
    }
}


void Tracer::renderTile(Tile &t) {

    for (int y = t.ymin; y < t.ymax; ++y) {
        for (int x = t.xmin; x < t.xmax; ++x) {
            Vec3 pixel = Filter::tent(x, y, width, height, s, this);
            int index = y * width + x;
            backBuffer[index] =
                    backBuffer[index] * ((frame - 1.0) / frame) + (pixel / frame);
        }

    }
}

void Tracer::postProcess() {

    PostProcess::bloom(backBuffer, displayBuffer, bloomBuffer, 15, height, width, 0.1);
    PostProcess::ReinhardTM(bloomBuffer, displayBuffer);
}

void Tracer::render() {

    //Frame is finished, display frame and prepare new frame
    if (tiles.size() == 0) {
        if (frame > 0) {
            frameTimer.print();
            Timer ppTime("Post process");
            postProcess();
            ppTime.print();
        }
        frame++;
        frameTimer.reset();
        buildTiles();
    }

    //Each thread renders one tile
    int threads = omp_get_max_threads();

    int startTile = std::max((int) tiles.size() - threads, 0);
    int endTile = tiles.size();

#pragma omp parallel for
    for (int i = startTile; i < endTile; i++) {
        renderTile(tiles[i]);
    }

    //Clear finished tiles
    tiles.erase(tiles.begin() + startTile, tiles.end());


}


Vec3 Tracer::trace(Ray ray) const {


    Hit hit = scene->bvhIntersect(ray);
    if (hit.IsValid()) {

        //Russian roulette
        if (ray.depth >= bounces) {
            //Use max reflectance as termination probability
            Vec3 c = hit.obj->albedo(hit.u, hit.v);
            float rr = std::max(c.x, std::max(c.y, c.z));
            //Don't have a reflectance of 1.0, otherwise we may never finish
            rr = std::min(rr,0.95f);
            if (Random(0, 1) < rr) {
                return hit.obj->material()->shade(hit, this) / rr;
            }
            else {
                return Vec3();
            }
        }
        else {

            return hit.obj->material()->shade(hit, this);
        }


    }
    else {
        return scene->env(ray.d);
    }
}