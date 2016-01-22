#include "Tracer.h"

#define RR_PROB 0.8

void Tracer::Render() {
    auto start = clock();

    frame++;
    buffer.swap(backbuffer);

#pragma omp parallel for
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Vec3 pixel;
            for (int xi = 0; xi < s; ++xi) {
                for (int yi = 0; yi < s; ++yi) {
                    float xp = (x * s + xi) / float(s * width);
                    float yp = (y * s + xi) / float(s * height);
                    Vec3 r = Trace(camera.GetRay(xp * 2 - 1, yp * 2 - 1));
                    pixel += Vec3(clamp(powf(r.x, 0.45f), 0.0f, 1.0f),
                                  clamp(powf(r.y, 0.45f), 0.0f, 1.0f),
                                  clamp(powf(r.z, 0.45f), 0.0f, 1.0f));
                }
            }
            buffer[y * width + x] =
                    backbuffer[y * width + x] * ((frame - 1.0) / frame) + ((pixel / float(s * s)) / frame);
        }

    }

    //printf("Complete in %0.3fs\n", float(clock() - start) / CLOCKS_PER_SEC);
}


Vec3 Tracer::Trace(Ray ray, int depth) {

    Vec3 colour;
    Vec3 reflectance(1);
    int i = 0;

    while (true) {

        Hit hit = scene.Intersect(ray);
        if (hit.IsValid()) {

            //Add emissive term only on first iteration
            if (i == 0) {
                colour += reflectance * hit.obj->material->colour * hit.obj->material->emmissive;
            }

            //Russian roulette
            float rr = 1.0;
            if (++i > depth) {

                if (Random(0, 1) < RR_PROB) {
                    rr /= RR_PROB;
                }
                else {
                    return colour;
                }
            }

            //Incoming direction from perspective of hit object
            Vec3 d = (-ray.d).Normalize();

            //Fresnel term - schlick's approximation
            float f0 = 0.17;
            float f_ = 1.0 - cos(hit.n.Dot(d));
            float fresnel = f0 + (1 - f0) * f_ * f_ * f_ * f_ * f_;


            //Accumulate direct lighting
            Vec3 direct;
            for (Object *light : scene.Lights()) {

                //Do not check light against itself
                if (light == hit.obj) {
                    continue;
                }


                //Create random direction towards sphere
                Vec3 sw = light->Position - hit.p;
                Vec3 su = ((fabs(sw.x) > 0.1 ? Vec3(0, 1, 0) : Vec3(1)).Cross(sw)).Normalize();
                Vec3 sv = sw.Cross(su);

                float cos_a_max = sqrt(
                        1.0 -
                        light->Radius * light->Radius / (hit.p - light->Position).Dot(hit.p - light->Position));

                float eps1 = Random(0, 1);
                float cos_a = 1 - eps1 + eps1 * cos_a_max;
                float sin_a = sqrt(1 - cos_a * cos_a);
                float phi = Random(0, 2 * M_PI);
                Vec3 l = (su * cos(phi) * sin_a + sv * sin(phi) * sin_a + sw * cos_a).Normalize();

                //Don't bother testing if normal points away from light
                if (l.Dot(hit.n) < 0.0) {
                    continue;
                }

                //Test shadow Ray
                if (scene.IsVisible(hit.p + hit.n * EPS, l, light)) {

                    //Solid angle
                    float omega = 2 * M_PI * (1.0 - cos_a_max);


                    direct += hit.obj->material->brdf(d, hit.n, l, fresnel) * light->material->emmissive *
                              light->material->colour * omega *
                              l.Dot(hit.n);

                }
            }


            reflectance *= hit.obj->material->colour * rr;
            colour += reflectance * direct;

            //Weight corrects for the PDF
            float weight = 1.0;
            Vec3 newDir = hit.obj->material->bounce(d, hit.n, weight, fresnel);
            reflectance *= weight;

            //If contribution will be zero we can just stop
            if (reflectance.Length() < EPS) {
                return colour;
            }

            ray = {hit.p + newDir * EPS, newDir};

        }
        else {
            return colour;
        }
    }
    return colour;
}