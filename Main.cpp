#include "Includes.h"
#include "Tracer.h"
#include <sstream>
#include "Material.h"


Tracer tracer;

//Materials
std::unique_ptr<Lambert> white(new Lambert(Vec3(0.75), 0.0));
std::unique_ptr<FresnelBlend> gloss(new FresnelBlend(Vec3(0.75, 0.25, 0.25), 0.0, 1000.0, 0.05));
std::unique_ptr<Lambert> e(new Lambert(Vec3(1), 60.0));
std::unique_ptr<Lambert> e_debug(new Lambert(Vec3(0.5), 0.25));
std::unique_ptr<Lambert> red(new Lambert(Vec3(0.75, 0.25, 0.25), 0.0));
std::unique_ptr<Lambert> blue(new Lambert(Vec3(0.25, 0.25, 0.75), 0.0));
std::unique_ptr<Lambert> black(new Lambert(Vec3(0.75), 0.0));

void debug1() {

    tracer.SetCamera(Vec3(0.0, 0.0, -18.0), Vec3(0, 0, 0), Vec3(0, 1, 0), M_PI / 13);
    tracer.Add(Vec3(0, -1.0, 0), 1.0, white.get());//Ball 1
    tracer.Add(Vec3(0, 1.0, 0), 1.0, e.get());//Light

    tracer.Init();
}

void debug2() {

    tracer.SetCamera(Vec3(0.0, 0.0, 0.0), Vec3(0, 0, 2), Vec3(0, 1, 0), M_PI / 13);
    tracer.Add(Vec3(0, -1e4, 0), 1e4 - 1, e_debug.get());
    tracer.Add(Vec3(0, 1e4, 0), 1e4 - 1, e_debug.get());
    tracer.Add(Vec3(-1e4, 0, 0), 1e4 - 1, e_debug.get());
    tracer.Add(Vec3(1e4, 0, 0), 1e4 - 1, e_debug.get());
    tracer.Add(Vec3(0, 0, -1e4), 1e4 - 1, e_debug.get());
    tracer.Add(Vec3(0, 0, 1e4), 1e4 - 1, e_debug.get());

    tracer.Init();
}

void hdrScene() {

    tracer.SetCamera(Vec3(-1, -1.5, -18.0), Vec3(0, 5, 0), Vec3(0, 1, 0), M_PI / 13);


    //tracer.Add(Vec3(0, -1e4, 0), 1e4, white->get()); //Bottom
    tracer.Add(Vec3(2.0, 1.0, 0), 1.0,  gloss.get());//Ball 1
    tracer.Add(Vec3(-2, 1.0, -4.5), 1.0, gloss.get());//Ball 2

    tracer.setEnvMap("Texture/road.hdr", ENV_LATLNG);

    tracer.Init();
}

void initScene() {

    tracer.SetCamera(Vec3(0.0, 2.5, -18.0), Vec3(0, 2, 0), Vec3(0, 1, 0), M_PI / 5);


    tracer.Add(Vec3(1e4 + 3, 0, 0), 1e4, red.get()); //Left
    tracer.Add(Vec3(-1e4 - 3, 0, 0), 1e4, blue.get()); //Right
    tracer.Add(Vec3(0, 0, 1e4 + 3), 1e4, white.get()); //Back
    tracer.Add(Vec3(0, 0, -1e4 - 20), 1e4, black.get()); //Front
    //tracer.Add(Vec3(0, 1e4 + 5, 0), 1e4, white.get()); //Top

    tracer.Add(Vec3(0, -1e4, 0), 1e4, white.get()); //Bottom
    tracer.Add(Vec3(2.0, 1.0, 0), 1.0, gloss.get());//Ball 1
    tracer.Add(Vec3(-2, 1.0, -4.5), 1.0, gloss.get());//Ball 2
    tracer.Add(Vec3(-1, 4.5, -2), 0.5, e.get());//Light


    tracer.Init();
}


void bvhTest() {

    tracer.SetCamera(Vec3(0.0, 2.5, -18.0), Vec3(0, 2, 0), Vec3(0, 1, 0), M_PI / 13);

    tracer.Add(Vec3(-3, 4.5, -2), 0.5, e.get());//Light

    int spheres_x = 100;
    int spheres_y = 10;
    for (int y = 0; y < spheres_y; y++) {
        for (int x = 0; x < spheres_x; x++) {

            tracer.Add(Vec3(x * 5, y * 5, 0), 0.5, white.get());
        }
    }
    tracer.Init();
}


void benchmark(int iterations) {
    size_t start = clock();
    for (int i = 0; i < iterations; i++) {
        tracer.Render();
    }
    printf("Benchmark time: %1.2f\n", ((double) clock() - start) / CLOCKS_PER_SEC);
}

void display() {

    tracer.Render();
    glDrawPixels(tracer.Width(), tracer.Height(), GL_RGB, GL_FLOAT, tracer.GetImage());
    std::ostringstream ss;
    ss << "Tracer. Samsples per pixel: " << tracer.getSamplesPP();
    std::string title = ss.str();
    glutSetWindowTitle(title.c_str());
    glutSwapBuffers();
}


int main(int argc, char **argv) {
    int w = 1000, h = 1000, s = 9, b = 2, ts = 32;

    bool bench = false;
    int bench_iterations = 100;
    bool db1 = false;
    bool db2 = false;

    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "-s") == 0) {
            s = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-w") == 0) {
            w = atoi(argv[++i]);
        } else if (strcmp(argv[i], "-h") == 0) {
            h = atoi(argv[++i]);
        }
    }

    tracer = Tracer(w, h, s, b, ts);

    if (db1) {
        debug1();
    }
    else if (db2) {
        debug2();
    }
    else if (bench) {
        initScene();
        benchmark(bench_iterations);
        return 0;
    }
    else {
        //initScene();
        hdrScene();
    }

    glutInit(&argc, argv);
    glutInitWindowSize(w, h);
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
    glutCreateWindow("Tracer");
    glutDisplayFunc(display);
    glutIdleFunc(display);

    glutMainLoop();

    return 0;
}