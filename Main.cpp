#include "Includes.h"
#include "Tracer.h"

Tracer tracer;

void initScene() {

    tracer.SetCamera(Vec3(0.0, 2.5, -18.0), Vec3(0, 2, 0), Vec3(0, 1, 0));

    auto white = new std::unique_ptr<Lambert>(new Lambert(Vec3(0.75), 0.0));
    auto red = new std::unique_ptr<Lambert>(new Lambert(Vec3(0.75, 0.25, 0.25), 0.0));
    auto blue = new std::unique_ptr<Lambert>(new Lambert(Vec3(0.25, 0.25, 0.75), 0.0));
    auto green = new std::unique_ptr<Lambert>(new Lambert(Vec3(0.25, 0.75, 0.25), 0.0));
    auto e = new std::unique_ptr<Lambert>(new Lambert(Vec3(1), 60.0));
    auto black = new std::unique_ptr<Lambert>(new Lambert(Vec3(0.75), 0.0));
    auto gloss = new std::unique_ptr<MPhong>(new MPhong(Vec3(0.1, 0.1, 0.75), 0.0, 1000.0));

    tracer.Add(Vec3(1e4 + 3, 0, 0), 1e4, white->get()); //Left
    tracer.Add(Vec3(-1e4 - 3, 0, 0), 1e4, white->get()); //Right
    tracer.Add(Vec3(0, 0, 1e4 + 3), 1e4, white->get()); //Back
    tracer.Add(Vec3(0, 0, -1e4 - 20), 1e4, black->get()); //Front
    tracer.Add(Vec3(0, 1e4 + 5, 0), 1e4, white->get()); //Top

    tracer.Add(Vec3(0, -1e4, 0), 1e4, white->get()); //Bottom
    tracer.Add(Vec3(2.0, 1.0, 0), 1.0, white->get());//Ball 1
    tracer.Add(Vec3(-2, 1.0, -4.5), 1.0, gloss->get());//Ball 2
    tracer.Add(Vec3(0, 4.5, -2), 0.2, e->get());//Light

    tracer.Init();
}

void display() {

    tracer.Render();
    glDrawPixels(tracer.Width(), tracer.Height(), GL_RGB, GL_FLOAT, tracer.GetImage());
    glutSwapBuffers();
}

int main(int argc, char **argv) {
    int w = 500, h = 500, s = 8;


    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "-s") == 0) {
            s = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-w") == 0) {
            w = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-h") == 0) {
            h = atoi(argv[++i]);
        }
    }

    tracer = Tracer(w, h, s);
    initScene();

    glutInit(&argc, argv);
    glutInitWindowSize(w, h);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutCreateWindow("Tracer");
    glutDisplayFunc(display);
    glutIdleFunc(display);

    glutMainLoop();

    return 0;
}