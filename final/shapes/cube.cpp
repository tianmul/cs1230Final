#include "cube.h"
#include "shape.h"
#include <iostream>
Cube::Cube():Shape()
{
}

void Cube::init(){
    //front face
    param1 = param1*2;
    for (int i = 0; i<param1; i++){

        Point a(-0.5,0.5-i*1./param1,0.5);
        Point b(0.5,0.5-i*1./param1,0.5);
        Point c(0.5,0.5-(i+1)*1./param1,0.5);
        if(i == 0) initSquare(a,b,c, true);
        else initSquare(a,b,c, false);
    }

    //right face
    for (int i = 0; i<param1; i++){
        Point a(0.5,0.5-i*1./param1,0.5);
        Point b(0.5,0.5-i*1./param1,-0.5);
        Point c(0.5,0.5-(i+1)*1./param1,-0.5);
        initSquare(a,b,c, false);
    }


    //back face
    for (int i = 0; i<param1; i++){
        Point a(0.5,0.5-i*1./param1,-0.5);
        Point b(-0.5,0.5-i*1./param1,-0.5);
        Point c(-0.5,0.5-(i+1)*1./param1,-0.5);
        initSquare(a,b,c, false);
    }

    //left face
    for (int i = 0; i<param1; i++){
        Point a(-0.5,0.5-i*1./param1,-0.5);
        Point b(-0.5,0.5-i*1./param1,0.5);
        Point c(-0.5,0.5-(i+1)*1./param1,0.5);
        initSquare(a,b,c, false);
    }

    //top face
    for (int i = 0; i<param1; i++){
        Point a(-0.5,0.5,-0.5+i*1./param1);
        Point b(0.5,0.5,-0.5+i*1./param1);
        Point c(0.5,0.5,-0.5+(i+1)*1./param1);
        initSquare(a,b,c, false);
    }

    //bot face
    for (int i = 0; i<param1; i++){
        Point a(-0.5,-0.5,0.5-i*1./param1);
        Point b(0.5,-0.5,0.5-i*1./param1);
        Point c(0.5,-0.5,0.5-(i+1)*1./param1);
        initSquare(a,b,c, false);
    }
}
