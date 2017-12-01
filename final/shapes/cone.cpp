#include "cone.h"

Cone::Cone():Shape()
{
}

void Cone::init(){
    Point tip(0,0.5,0);
    Point center(0,-0.5,0);
    initCirc(tip, center, 0.5, false, true);
    initCirc(center, center, 0.5, true, false);
}

