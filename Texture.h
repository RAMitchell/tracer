//
// Created by R on 19/02/2016.
//

#ifndef TRACER_TEXTURE_H
#define TRACER_TEXTURE_H


#include "Vector.h"

class Texture {
public:
     virtual Vec3 sample(float u,float v)=0;
};

 class Grid: public Texture{
     int u_num;
     int v_num;
     float x_width;
     float y_width;
     Vec3 line_colour;
     Vec3 back_colour;
 public:
     Grid(int u_num, int v_num, float x_width, float y_width, const Vec3 &line_colour, const Vec3 &back_colour) : u_num(
             u_num), v_num(v_num), x_width(x_width), y_width(y_width), line_colour(line_colour), back_colour(
             back_colour) { }

     Vec3 sample(float u,float v);
 };

#endif //TRACER_TEXTURE_H
