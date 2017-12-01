#include "cylinder.h"

Cylinder::Cylinder():Shape()
{
}

void Cylinder::init(){
    Point center1(0,0.5,0);
    Point center2(0,-0.5,0);
    initCirc(center1, center1, 0.5, false, true);
    initCirc(center2, center2, 0.5, true, false);


    Point p1(-0.5, 0.5, 0);
    float degree = 360./param2;
    Point p2(-0.5, -0.5, 0);
    Point p3 = p2.rotate(-degree);
    p3.y = -0.5;

    for (int i = 0; i < param2; i++){
        initcircSquare(p1,p2,p3,center1);
        p1 = p1.rotate(-degree);
        p2 = p2.rotate(-degree);
        p3 = p3.rotate(-degree);
    }


}
