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

    BBox() { }

    BBox(float x0, float x1, float y0, float y1, float z0, float z1) : x0(x0), x1(x1), y0(y0), y1(y1), z0(z0),
                                                                       z1(z1) { }

    bool hit(const Ray &r) const;

    void print() {
        printf("x0: %1.2f, x1: %1.2f, y0: %1.2f, y1: %1.2f, z0: %1.2f, z1: %1.2f\n", x0, x1, y0, y1, z0, z1);
    }
};

class BVH {

    class ArrayNode {

    public:
        int left = -1;
        int right = -1;
        BBox box;
        const Object *obj = NULL;

        ArrayNode() { }

        Hit intersect(const Ray&r,const std::vector< BVH::ArrayNode>& nodes)const;
    };

    class Node {
    public:
        std::unique_ptr<Node> left = NULL;
        std::unique_ptr<Node> right = NULL;
        BBox box;
        const Object *obj = NULL;

        Node(const std::vector<const Object *> &objects, int axis);

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
        BBox getBBox(const std::vector<const Object *> &objects);
    };

    std::unique_ptr<Node> root;
    std::vector<ArrayNode> nodes;

    void flatten(std::unique_ptr<Node> &n) {
        int currentIndex = nodes.size();
        nodes.push_back(ArrayNode());
        nodes[currentIndex].box = n->box;
        nodes[currentIndex].obj= n->obj;

        if (n->left) {
            nodes[currentIndex].left = nodes.size();
            flatten(n->left);
        }

        if (n->right) {
            nodes[currentIndex].right = nodes.size();
            flatten(n->right);
        }

    }

public:

    void build(std::vector<const Object *> objs) {
        try {
            //Build tree
            root = std::unique_ptr<Node>(new Node(objs, 0));

            //Flatten tree into array
            flatten(root);

            //Clean up tree
            root.reset();
        }
        catch(int e){
            std::cerr << "Error building bvh.\n";
        }
    }

    void print() {
        if (root != NULL) {
            root->print(0);
        }
    }

    Hit intersect(const Ray &r) const;
};


#endif //TRACER_BVH_H
