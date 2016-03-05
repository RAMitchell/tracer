#include "Includes.h"
#include "Tracer.h"
#include <sstream>


Tracer tracer;
Scene scene;


void addMaterials() {
    scene.addMaterial("white", new Lambert(Vec3(0.75)));
    scene.addMaterial("blue gloss", new FresnelBlend(Vec3(0.25, 0.25, 0.75), 0.0, 10000.0, 0.05));
    scene.addMaterial("e", new Lambert(Vec3(1), 120));
    scene.addMaterial("red", new Lambert(Vec3(0.75, 0.25, 0.25)));
    scene.addMaterial("blue", new Lambert(Vec3(0.25, 0.25, 0.75)));
    scene.addMaterial("black", new Lambert(Vec3(0)));
    scene.addMaterial("refract", new Refract(1.5));
    scene.addMaterial("mirror", new Mirror());
}

void hdrScene() {

    scene.setCamera(Vec3(0, 2.5, -10.0), Vec3(0.75, 1, 0), Vec3(0, 1, 0), M_PI / 13, 0.055, 7.15);

    OBJMesh mini("Models/minicooper.obj");
    mini.scale(0.025);
    mini.rotate(Vec3(1, 0, 0), -M_PI / 2);
    mini.rotate(Vec3(0, 0, 1), M_PI / 1.3);
    mini.translate(Vec3(0, 1, -2));
    mini.generateSmoothNormals();
    scene.addMesh(&mini,"blue gloss");
    scene.setEnvMap("Texture/road.hdr", ENV_LATLNG);

    tracer.loadScene(&scene);
}

/*
void dof() {

    tracer.setCamera(Vec3(0.0, 2.5, 0), Vec3(0, 2, 18), Vec3(0, 1, 0), M_PI / 13, 0.1, 20);
    tracer.addSphere(Sphere(Vec3(0, -1e4, 0), 1e4, white.get())); //Bottom
    for (int i = 0; i < 100; i++) {
        tracer.addSphere(Sphere(Vec3(-3, 1.0, i * 5), 1.0, gWhite.get()));
    }
    tracer.addSphere(Sphere(Vec3(-1, 4.5, 20), 0.5, e.get()));//Light
    tracer.setEnvMap(Vec3(3.0));
    tracer.Init();
}
*/


void initScene() {
    scene.setCamera(Vec3(0.0, 2.5, -18.0), Vec3(0, 2, 0), Vec3(0, 1, 0), M_PI / 13);

    scene.addSphere(Sphere(Vec3(1e4 + 3, 0, 0), 1e4), "red"); //Left
    scene.addSphere(Sphere(Vec3(-1e4 - 3, 0, 0), 1e4), "blue"); //Right
    scene.addSphere(Sphere(Vec3(0, 0, -1e4 - 20), 1e4), "black"); //Front
    scene.addSphere(Sphere(Vec3(0, 0, 1e4 + 5), 1e4), "white");//Front

    scene.addSphere(Sphere(Vec3(0, -1e4, 0), 1e4), "white");//Bottom
    scene.addSphere(Sphere(Vec3(-2, 1.0, -4.5), 1.0), "refract");//Ball 2
    scene.addSphere(Sphere(Vec3(-1, 4.5, -2), 0.5), "e");//Light


    OBJMesh mesh("Models/cow-nonormals.obj");
    mesh.scale(0.5);
    mesh.translate(Vec3(0, 1, -4));
    //scene.addMesh(&mesh, "blue gloss");

    BoxMesh box(1.5, 2.5, 0.2);
    box.translate(Vec3(0, 0, -3));
    box.rotate(Vec3(0,1,0),M_PI * 2-M_PI/4);
    scene.addMesh(&box, "refract");


    tracer.loadScene(&scene);
}


void display() {

    tracer.render();
    glDrawPixels(tracer.getWidth(), tracer.getHeight(), GL_RGB, GL_FLOAT, tracer.getImage());
    std::ostringstream ss;
    ss << "Tracer. Samples per pixel: " << tracer.getSamplesPP();
    std::string title = ss.str();
    glutSetWindowTitle(title.c_str());
    glutSwapBuffers();
}


int main(int argc, char **argv) {
    int w = 1000, h = 1000, samplesPP = 4, bounces = 3, tileSize = 32;


    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "-s") == 0) {
            samplesPP = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-w") == 0) {
            w = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-h") == 0) {
            h = atoi(argv[++i]);
        }
    }

    tracer = Tracer(w, h, samplesPP, bounces, tileSize);

    addMaterials();

    //initScene();
    hdrScene();
    //dof();

    glutInit(&argc, argv);
    glutInitWindowSize(w, h);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutCreateWindow("Tracer");
    glutDisplayFunc(display);
    glutIdleFunc(display);

    glutMainLoop();

    return 0;
}

