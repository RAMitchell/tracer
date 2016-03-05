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

BVH::Node::Node(const std::vector<const Object *> &objects, int axis) {

    box = getBBox(objects);
    if (objects.size() == 1) {
        obj = objects.front();
        return;
    }

    //Separate objects according to midpoint of current axis
    std::vector<const Object *> left_objects;
    std::vector<const Object *> right_objects;

    if (objects.size() == 2) {
        left_objects.push_back(objects.front());
        right_objects.push_back(objects.back());
    }
    else {
        //Find midpoint depending on axis and create left and right nodes depending on sub lists of objects
        if (axis == 0) {
            float midpoint = box.x0 + ((box.x1 - box.x0) / 2);
            std::copy_if(objects.begin(), objects.end(), std::back_inserter(left_objects),
                         [&](const Object *o) {
                             return o->centre().x < midpoint;
                         });
            std::copy_if(objects.begin(), objects.end(), std::back_inserter(right_objects), [&](const Object *o) {
                return o->centre().x >= midpoint;
            });

        }
        else if (axis == 1) {
            float midpoint = box.y0 + ((box.y1 - box.y0) / 2);
            std::copy_if(objects.begin(), objects.end(), std::back_inserter(left_objects), [=](const Object *o) {
                return o->centre().y < midpoint;
            });
            std::copy_if(objects.begin(), objects.end(), std::back_inserter(right_objects), [=](const Object *o) {
                return o->centre().y >= midpoint;
            });

        }
        else if (axis == 2) {
            float midpoint = box.z0 + ((box.z1 - box.z0) / 2);
            std::copy_if(objects.begin(), objects.end(), std::back_inserter(left_objects), [=](const Object *o) {
                return o->centre().z < midpoint;
            });
            std::copy_if(objects.begin(), objects.end(), std::back_inserter(right_objects), [=](const Object *o) {
                return o->centre().z >= midpoint;
            });

        }
    }
    //Handle the case where all objects are on the right or left
    if (right_objects.size() > 2 && left_objects.size() == 0) {
        int half = ceil(right_objects.size() / 2.0);
        //Copy half the items into left objects
        std::copy(right_objects.begin() + half, right_objects.end(), std::back_inserter(left_objects));
        right_objects.erase(right_objects.begin() + half, right_objects.end());
    }
    else if (left_objects.size() > 2 && right_objects.size() == 0) {
        int half = ceil(left_objects.size() / 2.0);
        //Copy half the items into left objects
        std::copy(left_objects.begin() + half, left_objects.end(), std::back_inserter(right_objects));
        left_objects.erase(left_objects.begin() + half, left_objects.end());
    }

    if (left_objects.size() > 0) {
        left = std::unique_ptr<Node>(new Node(left_objects, (axis + 1) % 3));
    }
    if (right_objects.size() > 0) {
        right = std::unique_ptr<Node>(new Node(right_objects, (axis + 1) % 3));
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

        if (obj != NULL) {
            Hit h3 = obj->intersect(r);
            if (h3.distance < h.distance) {
                h = h3;
            }
        }

    }

    return h;
}

Hit BVH::ArrayNode::intersect(const Ray &r, const std::vector<BVH::ArrayNode> &nodes) const {

    Hit h;
    if (box.hit(r)) {
        if (left >= 0) {
            Hit h1 = nodes[left].intersect(r, nodes);
            if (h1.distance < h.distance) {
                h = h1;
            }
        }

        if (right >= 0) {
            Hit h2 = nodes[right].intersect(r, nodes);
            if (h2.distance < h.distance) {
                h = h2;
            }
        }

        if (obj != NULL) {
            Hit h3 = obj->intersect(r);
            if (h3.distance < h.distance) {
                h = h3;
            }
        }

    }

    return h;
}

BBox BVH::Node::getBBox(const std::vector<const Object *> &objects) {

    const float min = -1e5, max = 1e5;
    BBox b(max, min, max, min, max, min);

    for (const Object *o: objects) {

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


    if (nodes.size() > 0) {
        return nodes.front().intersect(r, nodes);
    }
    else {
        return Hit();
    }
}
