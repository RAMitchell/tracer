//
// Created by R on 8/02/2016.
//

#ifndef TRACER_BVH_H
#define TRACER_BVH_H

#include "Camera.h"
#include <memory>
#include <vector>

class Object;

class Hit;

class BBox {
public:
    float x0, x1, y0, y1, z0, z1;

    BBox(){}

    BBox(float x0, float x1, float y0, float y1, float z0, float z1) : x0(x0), x1(x1), y0(y0), y1(y1), z0(z0),
                                                                       z1(z1) { }

    bool hit(const Ray &r) const;

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
        std::vector<const Object *> objs;

        Node(std::vector<const Object *> objects, int axis);

        Hit intersect(const Ray &r) const;

        void print(int depth) {
            printf("depth : %d\n", depth);
            box.print();
            if (left != NULL) {
                left->print(depth + 1);
            }
            if (right != NULL) {
                right->print(depth + 1);
            }
        }

    private:
        //Find the smallest bounding box that encompasses objects
        BBox getBBox(std::vector<const Object *> &objects);
    };

    std::unique_ptr<Node> root;

public:

    void build(std::vector<const Object*> objs) {
        root = std::unique_ptr<Node>(new Node(objs, 0));
    }

    void print() {
        if (root != NULL) {
            root->print(0);
        }
    }

    Hit intersect(const Ray &r) const ;
};


#endif //TRACER_BVH_H
