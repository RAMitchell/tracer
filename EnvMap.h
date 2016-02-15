//
// Created by R on 10/02/2016.
//

#ifndef TRACER_ENVMAP_H
#define TRACER_ENVMAP_H


#include "Vector.h"
#include "HDRloader/hdrloader.h"
#include "Includes.h"
#include <string>
#include <iostream>

enum envProjection {
    ENV_LATLNG,
    ENV_VCROSS
};

class EnvMap {

    Vec3 colour;
    bool use_texture;
    HDRLoaderResult hdr;
    envProjection proj;

public:
    EnvMap(Vec3 colour) : colour(colour) {
        use_texture = false;
    }

    EnvMap(std::string texture_filename, envProjection proj) : proj(proj) {
        use_texture = true;

        HDRLoader loader;
        if (!(loader.load(texture_filename.c_str(), hdr))) {
            std::cout << "Error loading environment map: " << texture_filename << "\n";
            use_texture = false;
        }

    }

    Vec3 sample(const Vec3 &d) const {
        if (!use_texture) {
            return colour;
        }

        switch (proj) {
            case ENV_VCROSS:
                return sampleVerticalCross(d);
            case ENV_LATLNG:
                return sampleLatLng(d);
            default:
                return colour;

        }
    }

private:
    Vec3 sampleLatLng(const Vec3 &d) const {

        float theta = acosf(d.y);
        float phi = atan2(d.z, d.x);
        float u = (M_PI + phi) / (2 * M_PI);
        float v = theta * M_1_PI;

        int x = clamp((int) (hdr.width * u), 0, hdr.width - 1);
        int y = clamp((int) (hdr.height * v), 0, hdr.height - 1);


        int index = ((hdr.width * y) + x) * 3;

        return Vec3(hdr.cols[index], hdr.cols[index + 1], hdr.cols[index + 2]);

    }

    Vec3 sampleVerticalCross(const Vec3 &d) const {
        int face_width = hdr.width / 3;
        int face_height = hdr.height / 4;

        //Pixel coordinates
        int x = 0, y = 0;

        if (fabs(d.z) > fabs(d.x) && fabs(d.z) > fabs(d.y)) {
            float scale = 1.0 / fabs(d.z);
            Vec3 vec = d * scale;

            //uv [0,1]^2
            float u = (vec.x + 1) / 2, v = (vec.y + 1) / 2;

            //Sample front face
            if (d.z >= 0) {

                x = face_width + (face_width * (1 - u));
                y = face_height + (face_height * (1 - v));

            }
                //Sample back face
            else {
                x = face_width + (face_width * (1 - u));
                y = (3 * face_height) + (face_height * v);
            }
        }
        else if (fabs(d.x) > fabs(d.y)) {

            float scale = 1.0 / fabs(d.x);
            Vec3 vec = d * scale;

            //uv [0,1]^2
            float u = (vec.z + 1) / 2, v = (vec.y + 1) / 2;

            //Sample  left face
            if (d.x >= 0) {

                x = (face_width * u);
                y = face_height + (face_height * (1 - v));

            }
                //Sample  right face
            else {
                x = face_width * 2 + (face_width * (1 - u));
                y = face_height + (face_height * (1 - v));
            }
        }
        else {

            float scale = 1.0 / fabs(d.y);
            Vec3 vec = d * scale;

            //uv [0,1]^2
            float u = (vec.x + 1) / 2, v = (vec.z + 1) / 2;

            //Sample top face
            if (d.y >= 0) {

                x = face_width + (face_width * (1 - u));
                y = (face_height * v);

            }
                //Sample bottom face
            else {
                x = face_width + (face_width * (1 - u));
                y = face_height * 2 + (face_height * (1 - v));
            }
        }

        int index = (y * hdr.width + x) * 3;
        return Vec3(hdr.cols[index], hdr.cols[index + 1], hdr.cols[index + 2]) * 10;
    }


};


#endif //TRACER_ENVMAP_H
