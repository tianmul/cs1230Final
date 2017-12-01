#ifndef SPHERE_H
#define SPHERE_H
#include "shape.h"

class Sphere : public Shape
{
public:
    Sphere();
    virtual void setting(int p1, int p2, float p3);
    virtual void init();

};

#endif // SPHERE_H


