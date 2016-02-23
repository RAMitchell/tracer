//
// Created by R on 8/02/2016.
//

#include "BVH.h"
#include "Hit.h"
#include "Object.h"

bool BBox::hit(const Ray &r) const {
    float t1 = (x0 - r.o.x) * r.d_inv.x;
    float t2 = (x1 - r.o.x) * r.d_inv.x;

    float tmin = std::min(t1, t2);
    float tmax = std::max(t1, t2);

    t1 = (y0 - r.o.y) * r.d_inv.y;
    t2 = (y1 - r.o.y) * r.d_inv.y;

    tmin = std::max(tmin, std::min(std::min(t1, t2), tmax));
    tmax = std::min(tmax, std::max(std::max(t1, t2), tmin));

    t1 = (z0 - r.o.z) * r.d_inv.z;
    t2 = (z1 - r.o.z) * r.d_inv.z;

    tmin = std::max(tmin, std::min(std::min(t1, t2), tmax));
    tmax = std::min(tmax, std::max(std::max(t1, t2), tmin));

    return tmax > std::max(tmin, 0.0f);
}

BVH::Node::Node(std::vector<const Object *> objects, int axis) {

    box = getBBox(objects);
    if (objects.size() <= 2) {
        objs = objects;
        return;

    }

    //Separate objects according to midpoint of current axis
    std::vector<const Object *> left_objects;
    std::vector<const Object *> right_objects;

    //Find midpoint depending on axis and create left and right nodes depending on sub lists of objects
    if (axis == 0) {
        float midpoint = box.x0 + ((box.x1 - box.x0) / 2);
        std::copy_if(objects.begin(), objects.end(), std::back_inserter(left_objects),
                     [&](const Object *o) {
                         return o->position().x < midpoint;
                     });
        std::copy_if(objects.begin(), objects.end(), std::back_inserter(right_objects), [&](const Object *o) {
            return o->position().x >= midpoint;
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
            return o->position().y < midpoint;
        });
        std::copy_if(objects.begin(), objects.end(), std::back_inserter(right_objects), [=](const Object *o) {
            return o->position().y >= midpoint;
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
            return o->position().z < midpoint;
        });
        std::copy_if(objects.begin(), objects.end(), std::back_inserter(right_objects), [=](const Object *o) {
            return o->position().z >= midpoint;
        });

        if (left_objects.size() > 0) {
            left = std::unique_ptr<Node>(new Node(left_objects, (axis + 1) % 3));
        }
        if (right_objects.size() > 0) {
            right = std::unique_ptr<Node>(new Node(right_objects, (axis + 1) % 3));
        }
    }


}

Hit BVH::Node::intersect(const Ray &r) const {

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

        if (objs.size() > 0) {
            for (auto &obj:objs) {
                Hit h3 = obj->intersect(r);
                if (h3.distance < h.distance) {
                    h = h3;
                }
            }
        }

    }

    return h;
}

BBox BVH::Node::getBBox(std::vector<const Object *> &objects) {
    const float min = -1e5, max = 1e5;
    BBox b(max, min, max, min, max, min);

    for (const Object* o: objects) {

        BBox oBox = o->getBB();

        b.x0 = std::min(b.x0, oBox.x0);
        b.x1 = std::max(b.x1, oBox.x1);
        b.y0 = std::min(b.y0, oBox.y0);
        b.y1 = std::max(b.y1, oBox.y1);
        b.z0 = std::min(b.z0, oBox.z0);
        b.z1 = std::max(b.z1, oBox.z1);
    }

    return b;
}

Hit BVH::intersect(const Ray &r) const {

    if (root != NULL) {
        return root->intersect(r);
    }
    else {
        return Hit();
    }
}
