//
// Created by R on 25/02/2016.
//

#include <sstream>
#include "Object.h"
#include "Mesh.h"

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
        if (!item.empty()) {
            elems.push_back(item);
        }
    }
    return elems;
}


std::vector<std::string> split(const std::string &s, char delim) {
    std::vector<std::string> elems;
    split(s, delim, elems);
    return elems;
}

Mesh::Mesh(Mesh const &other) : mat(other.mat), p(other.p), trans(other.trans), vertices(other.vertices),
                                normals(other.normals), triangles(other.triangles) {
    for (Triangle &t: triangles) {
        t.mesh = this;
    }
}


OBJMesh::OBJMesh(std::string filename) {
    try {
        std::ifstream f(filename);


        if (f.is_open()) {

            int group = 0;
            std::string line;
            while (getline(f, line)) {

                std::vector<std::string> tokens = split(line, ' ');

                if (tokens.size() == 0) {
                    continue;
                }

                if (tokens[0] == "g") {
                    group++;
                }
                else if (tokens[0] == "v") {
                    Vec3 vertex(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()));
                    vertices.push_back(vertex);
                }
                else if (tokens[0] == "vn") {
                    Vec3 normal(atof(tokens[1].c_str()), atof(tokens[2].c_str()), atof(tokens[3].c_str()));
                    normal.Normalize();
                    normals.push_back(normal);
                }
                else if (tokens[0] == "f") {
                    if (line.find("//") != std::string::npos) {

                        std::vector<std::string> s1 = split(tokens[1], '/');
                        std::vector<std::string> s2 = split(tokens[2], '/');
                        std::vector<std::string> s3 = split(tokens[3], '/');

                        int p1 = atoi(s1[0].c_str()) - 1;
                        int p2 = atoi(s2[0].c_str()) - 1;
                        int p3 = atoi(s3[0].c_str()) - 1;
                        int n1 = atoi(s1[1].c_str()) - 1;
                        int n2 = atoi(s2[1].c_str()) - 1;
                        int n3 = atoi(s3[1].c_str()) - 1;

                        triangles.push_back(Triangle(this, p1, p2, p3, n1, n2, n3));
                    }
                    else if (line.find("/") != std::string::npos) {
                        std::vector<std::string> s1 = split(tokens[1], '/');
                        std::vector<std::string> s2 = split(tokens[2], '/');
                        std::vector<std::string> s3 = split(tokens[3], '/');

                        int p1 = atoi(s1[0].c_str()) - 1;
                        int p2 = atoi(s2[0].c_str()) - 1;
                        int p3 = atoi(s3[0].c_str()) - 1;
                        int n1 = atoi(s1[2].c_str()) - 1;
                        int n2 = atoi(s2[2].c_str()) - 1;
                        int n3 = atoi(s3[2].c_str()) - 1;

                        triangles.push_back(Triangle(this, p1, p2, p3, n1, n2, n3));
                    }
                    else {
                        int p1 = atoi(tokens[1].c_str()) - 1;
                        int p2 = atoi(tokens[2].c_str()) - 1;
                        int p3 = atoi(tokens[3].c_str()) - 1;

                        triangles.push_back(Triangle(this, p1, p2, p3, p1, p2, p3));
                    }

                }
            }
            f.close();
        }
        else {
            std::cerr << "OBJMesh: Unable to open file " << filename << "\n";
        }
    }
    catch (int e) {
        std::cerr << "Error parsing obj file.\n";
    }

    if (normals.size() == 0) {
        generateSmoothNormals();
    }


}

//Generate one normal for each vertex
void  Mesh::generateSmoothNormals() {

    normals = std::vector<Vec3>(vertices.size());

    for (int i = 0; i < triangles.size(); i++) {
        Triangle t = triangles[i];
        Vec3 n = Normalize((getVertex(t.p2) - getVertex(t.p1)).Cross(getVertex(t.p3) - getVertex(t.p1)));

        //Deal with degenerate triangles which have zero area
        if (std::isnan(n.x)) {
            n = Vec3(0, 1, 0);
        }

        triangles[i].n1 = t.p1;
        triangles[i].n2 = t.p2;
        triangles[i].n3 = t.p3;

        normals[t.p1] += n;
        normals[t.p2] += n;
        normals[t.p3] += n;
    }

    for (int i = 0; i < normals.size(); i++) {
        normals[i].Normalize();
    }
}

//Generate one normal per face
void  Mesh::generateFlatNormals() {
    normals = std::vector<Vec3>(triangles.size());


    for (int i = 0; i < triangles.size(); i++) {
        Triangle t = triangles[i];
        Vec3 n = Normalize((getVertex(t.p2) - getVertex(t.p1)).Cross(getVertex(t.p3) - getVertex(t.p1)));

        //Deal with degenerate triangles which have zero area
        if (std::isnan(n.x)) {
            n = Vec3(0, 1, 0);
        }

        normals[i] = n;

        triangles[i].n1 = i;
        triangles[i].n2 = i;
        triangles[i].n3 = i;

    }

}

BoxMesh::BoxMesh(float x, float y, float z) {
    vertices.push_back(Vec3(0, 0, 0));
    vertices.push_back(Vec3(x, 0, 0));
    vertices.push_back(Vec3(x, 0, z));
    vertices.push_back(Vec3(0, 0, z));
    vertices.push_back(Vec3(0, y, 0));
    vertices.push_back(Vec3(x, y, 0));
    vertices.push_back(Vec3(x, y, z));
    vertices.push_back(Vec3(0, y, z));

    triangles.push_back(Triangle(this, 0, 4, 1, 0, 0, 0));
    triangles.push_back(Triangle(this, 4, 5, 1, 0, 0, 0));
    triangles.push_back(Triangle(this, 5, 2, 1, 0, 0, 0));
    triangles.push_back(Triangle(this, 5, 6, 2, 0, 0, 0));
    triangles.push_back(Triangle(this, 6, 7, 3, 0, 0, 0));
    triangles.push_back(Triangle(this, 7, 3, 2, 0, 0, 0));
    triangles.push_back(Triangle(this, 7, 4, 3, 0, 0, 0));
    triangles.push_back(Triangle(this, 4, 0, 3, 0, 0, 0));
    triangles.push_back(Triangle(this, 6, 5, 4, 0, 0, 0));
    triangles.push_back(Triangle(this, 7, 6, 4, 0, 0, 0));
    triangles.push_back(Triangle(this, 2, 1, 0, 0, 0, 0));
    triangles.push_back(Triangle(this, 2, 0, 3, 0, 0, 0));

    generateFlatNormals();
}
