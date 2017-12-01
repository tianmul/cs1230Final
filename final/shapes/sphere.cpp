#include "sphere.h"
#include "math.h"
#define PI 3.14159265

Sphere::Sphere():Shape()
{
}
void Sphere::setting(int p1, int p2, float p3){
    param1 = p1 > 2 ? p1 : 2;
    param2 = p2 > 3 ? p2 : 3;
    param3 = p3;
}


void Sphere::init(){

    std::vector<Point> points;
    float radius = 0.5;
    float degree = PI/param1;

    for (float i = 1; i < param1; i++){

        points.push_back(Point(-radius*sin(i*degree), radius*cos(i*degree), 0));
    }

    Point topcenter(0,points[0].y,0);
    Point botcenter(0,points[points.size()-1].y,0);

    initCirc(Point(0,radius,0), topcenter, sqrt(radius*radius-topcenter.y*topcenter.y), false, true, true);

    if(param1 > 2){
        initcircSquareSP(points);
    }
    initCirc(Point(0,-radius,0), botcenter, sqrt(radius*radius-botcenter.y*botcenter.y), true, false, true);
}

