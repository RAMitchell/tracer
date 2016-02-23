//
// Created by R on 19/02/2016.
//

#include "Texture.h"

Vec3 Grid::sample(float u, float v) {

    float ulocal = std::fmod(u, 1.0f / u_num);
    float vlocal = std::fmod(v, 1.0f / v_num);

    float yline = y_width / 2;
    float xline = x_width / 2;

    if (ulocal < yline || ulocal > (1.0f / u_num) - yline) {
        return line_colour;
    }

    if(vlocal<xline||vlocal>(1.0f/v_num) - xline){
        return line_colour;
    }

    return back_colour;
}
