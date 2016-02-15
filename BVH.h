//
// Created by R on 8/02/2016.
//

#ifndef TRACER_BVH_H
#define TRACER_BVH_H

#include "Camera.h"
#include <memory>
#include <vector>
#include <Object.h>

class Hit;

class BBox {
public:
    BBox(float x0, float x1, float y0, float y1, float z0, float z1) : x0(x0), x1(x1), y0(y0), y1(y1), z0(z0),
                                                                       z1(z1) { }

    bool hit(const Ray &r) const {
        float tx_min, ty_min, tz_min;
        float tx_max, ty_max, tz_max;

        float a = 1.0f / r.d.x;
        if (a >= 0) {
            tx_min = (x0 - r.o.x) * a;
            tx_max = (x1 - r.o.x) * a;
        }
        else {
            tx_min = (x1 - r.o.x) * a;
            tx_max = (x0 - r.o.x) * a;
        }

        float b = 1.0f / r.d.y;
        if (b >= 0l) {
            ty_min = (y0 - r.o.y) * b;
            ty_max = (y1 - r.o.y) * b;

        }
        else {
            ty_min = (y1 - r.o.y) * b;
            ty_max = (y0 - r.o.y) * b;
        }

        float c = 1.0f / r.d.z;
        if (c >= 0) {
            tz_min = (z0 - r.o.z) * c;
            tz_max = (z1 - r.o.z) * c;
        }
        else {
            tz_min = (z1 - r.o.z) * c;
            tz_max = (z0 - r.o.z) * c;
        }

        float t0, t1;
        if (tx_min > ty_min) {
            t0 = tx_min;
        }
        else {
            t0 = ty_min;
        }
        if (tz_min > t0) {
            t0 = tz_min;
        }

        if (tx_max < ty_max) {
            t1 = tx_max;
        }
        else {
            t1 = ty_max;
        }
        if (tz_max < t1) {
            t1 = tz_max;
        }

        return (t0 < t1 && t1 > EPS);

    }

    float x0, x1, y0, y1, z0, z1;

    void print() {
        printf("x0: %1.2f, x1: %1.2f, y0: %1.2f, y1: %1.2f, z0: %1.2f, z1: %1.2f\n", x0, x1, y0, y1, z0, z1);
    }
};

class BVH {


    class Node {
    public:
        std::unique_ptr<Node> left = NULL;
        std::unique_ptr<Node> right = NULL;
        BBox box;
        const Object *obj = NULL;

        Node(std::vector<const Object *> objects, int axis) : box(getBBox(objects)) {
            if (objects.size() == 1) {
                obj = objects.back();
                return;

            }

            //Separate objects according to midpoint of current axis
            std::vector<const Object *> left_objects;
            std::vector<const Object *> right_objects;

            //If there are just two objects set them as left and right irrespective of their axis
            if (objects.size() == 2) {
                left_objects.push_back(objects[0]);
                right_objects.push_back(objects[1]);
                left = std::unique_ptr<Node>(new Node(left_objects, ++axis % 3));
                right = std::unique_ptr<Node>(new Node(right_objects, ++axis % 3));
                return;
            }

            //Find midpoint depending on axis and create left and right nodes depending on sub lists of objects
            if (axis == 0) {
                float midpoint = box.x0 + ((box.x1 - box.x0) / 2);
                std::copy_if(objects.begin(), objects.end(), std::back_inserter(left_objects),
                             [&](const Object *o) {
                                 return o->Position.x < midpoint;
                             });
                std::copy_if(objects.begin(), objects.end(), std::back_inserter(right_objects), [&](const Object *o) {
                    return o->Position.x >= midpoint;
                });

                if (left_objects.size() > 0) {
                    left = std::unique_ptr<Node>(new Node(left_objects, (axis + 1) % 3));
                }
                if (right_objects.size() > 0) {
                    right = std::unique_ptr<Node>(new Node(right_objects, (axis + 1) % 3));
                }
            }
            else if (axis == 1) {
                float midpoint = box.y0 + ((box.y1 - box.y0) / 2);
                std::copy_if(objects.begin(), objects.end(), std::back_inserter(left_objects), [=](const Object *o) {
                    return o->Position.y < midpoint;
                });
                std::copy_if(objects.begin(), objects.end(), std::back_inserter(right_objects), [=](const Object *o) {
                    return o->Position.y >= midpoint;
                });

                if (left_objects.size() > 0) {
                    left = std::unique_ptr<Node>(new Node(left_objects, (axis + 1) % 3));
                }
                if (right_objects.size() > 0) {
                    right = std::unique_ptr<Node>(new Node(right_objects, (axis + 1) % 3));
                }
            }
            else if (axis == 2) {
                float midpoint = box.z0 + ((box.z1 - box.z0) / 2);
                std::copy_if(objects.begin(), objects.end(), std::back_inserter(left_objects), [=](const Object *o) {
                    return o->Position.z < midpoint;
                });
                std::copy_if(objects.begin(), objects.end(), std::back_inserter(right_objects), [=](const Object *o) {
                    return o->Position.z >= midpoint;
                });

                if (left_objects.size() > 0) {
                    left = std::unique_ptr<Node>(new Node(left_objects, (axis + 1) % 3));
                }
                if (right_objects.size() > 0) {
                    right = std::unique_ptr<Node>(new Node(right_objects, (axis + 1) % 3));
                }
            }


        }

        Hit intersect(const Ray &r) const {

            Hit h;
            if (box.hit(r)) {
                if (left != NULL) {
                    Hit h1 = left->intersect(r);
                    if (h1.distance < h.distance) {
                        h = h1;
                    }
                }

                if (right != NULL) {
                    Hit h2 = right->intersect(r);
                    if (h2.distance < h.distance) {
                        h = h2;
                    }
                }

                if (obj != NULL) {
                    Hit h3 = obj->Intersect(r);
                    if (h3.distance < h.distance) {
                        h = h3;
                    }
                }

            }

            return h;
        }

        void print(int depth) {
             printf("depth : %d\n", depth);
            box.print();
            if (left != NULL) {
                left->print( depth +1);
            }
            if (right != NULL) {
                right->print(depth+1);
            }
        }

    private:
        //Find the smallest bounding box that encompasses objects
        BBox getBBox(std::vector<const Object *> &objects) {
            const float min = -1e5, max = 1e5;
            float x0 = max, x1 = min, y0 = max, y1 = min, z0 = max, z1 = min;
            for (auto o: objects) {
                float maxRadius = o->getMaxRadius();

                x0 = o->Position.x - maxRadius < x0 ? o->Position.x - maxRadius : x0;
                x1 = o->Position.x + maxRadius > x1 ? o->Position.x + maxRadius : x1;
                y0 = o->Position.y - maxRadius < y0 ? o->Position.y - maxRadius : y0;
                y1 = o->Position.y + maxRadius > y1 ? o->Position.y + maxRadius : y1;
                z0 = o->Position.z - maxRadius < z0 ? o->Position.z - maxRadius : z0;
                z1 = o->Position.z + maxRadius > z1 ? o->Position.z + maxRadius : z1;
            }

            return BBox(x0, x1, y0, y1, z0, z1);
        }
    };

    std::unique_ptr<Node> root;

public:

    void build(const std::vector<Object> &objs) {
        std::vector<const Object *> objects;
        for (const Object &o:objs) {
            objects.push_back(&o);
        }

        root = std::unique_ptr<Node>(new Node(objects, 0));
    }

    void print() {
        if (root != NULL) {
            root->print(0);
        }
    }

    Hit intersect(const Ray &r) const {

        if(root != NULL ) {
            return root->intersect(r);
        }
        else{
            return Hit();
        }
    }
};


#endif //TRACER_BVH_H
