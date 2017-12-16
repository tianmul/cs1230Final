#ifndef SHAPE_H
#define SHAPE_H
#include "shapes/openglshape.h"
#include "point.h"

class Shape
{
public:
    Shape();
    virtual void setting(int p1, int p2, float p3);
    virtual void init() = 0;
    void build();
    void draw();

protected:
    void initSquare(Point startP, Point v1P, Point endP, bool first);

    void initTri(Point tip, Point leftP, Point rightP, Point center, bool first, bool sp);

    void initCirc(Point tip, Point center, float r, bool clockwise, bool first, bool sp = false);

    void initcircSquare(Point startP, Point v1P, Point endP, Point center);
    /*
    void initCircSP(Point tip, Point center, float r, bool ctclockwise);*/
    void initcircSquareSP(std::vector<Point> points);


    void pushPoint(std::vector<float> &a, Point b);

    int param1;
    int param2;
    int param3;
    std::unique_ptr<OpenGLShape> theShape;
    std::vector<float> shapePoints;

};

#endif // SHAPE_H
