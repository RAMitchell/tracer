//
// Created by R on 25/02/2016.
//

#ifndef TRACER_MESH_H
#define TRACER_MESH_H

#include <fstream>
#include <map>
#include "Vector.h"
#include "Material.h"
#include "Mat3.h"

class Triangle;


class Mesh {
protected:
    const Material *mat;
    Vec3 p;
    Mat3 trans;
    std::vector<Vec3> vertices;
    std::vector<Vec3> normals;
    std::vector<Triangle> triangles;

    Vec3 transform(Vec3 v) {
        return (trans * v) + p;
    }

public:

    Mesh() { }

    Mesh(Mesh const &other);

    virtual Mesh *clone() = 0;

    void setMaterial(Material *m) {
        mat = m;
    }

    const Material *getMaterial() {
        return mat;
    }

    void translate(Vec3 t) {
        p += t;
    }

    void scale(Vec3 s) {
        trans.scale(s);
    }

    //Angle in radians
    void rotate(Vec3 axis, float angle) {
        trans.rotate(axis, angle);
    }

    std::vector<Triangle> &getTriangles() {
        return triangles;
    }

    Vec3 getVertex(int i) {
        return transform(vertices[i]);
    }

    Vec3 getNormal(int i) {
        return normals[i];
    }


    void generateSmoothNormals();

    void generateFlatNormals();

};

class OBJMesh : public Mesh {
public:
    OBJMesh(std::string filename);

    Mesh *clone() {
        return new OBJMesh(*this);
    }
};

class BoxMesh : public Mesh {
public:
    BoxMesh(float x, float y, float z);

    Mesh *clone() {
        return new BoxMesh(*this);
    }
};

#endif //TRACER_MESH_H
