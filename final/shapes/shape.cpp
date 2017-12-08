#include "shape.h"
#include "gl/shaders/ShaderAttribLocations.h"
#include <iostream>
#include <math.h>

Shape::Shape()
{
    theShape = std::make_unique<OpenGLShape>();
}

void Shape::setting(int p1, int p2, float p3){
    param1 = p1 > 1 ? p1 : 1;
    param2 = p2 > 3 ? p2 : 3;
    param3 = p3;
}


void Shape::draw(){
    theShape->setVertexData(&shapePoints[0], shapePoints.size(), VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, shapePoints.size()/6);
    theShape->setAttribute(ShaderAttrib::POSITION , 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    theShape->setAttribute(ShaderAttrib::NORMAL , 3, 12, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    theShape->setAttribute(ShaderAttrib::TEXCOORD0 , 2, 24, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    theShape->buildVAO();
    theShape->draw();
}

void Shape::initSquare(Point startP, Point v1P, Point endP, bool first){
    Point vec1 = (v1P-startP)/param1;
    Point vec2 = endP-v1P;

    Point dirvec = vec2.cross(vec1);
    Point afternor = dirvec.normal();

    for (int i=0; i<=param1; i++){
        Point p1 = startP + vec1*i;
        Point p2 = startP + vec2 + vec1*i;
        if(!first && i==0){
            pushPoint(shapePoints,p1);
            pushPoint(shapePoints,afternor);
        }
        pushPoint(shapePoints,p1);
        pushPoint(shapePoints,afternor);
        pushPoint(shapePoints,p2);
        pushPoint(shapePoints,afternor);
        if(i==param1) {
            pushPoint(shapePoints,p2);
            pushPoint(shapePoints,afternor);
        }
    }

}


void Shape::initTri(Point tip, Point leftP, Point rightP, Point center, bool first, bool sp){

    int max = sp?1:param1;
    Point vec1 = (leftP-tip)/max;
    Point vec2 = (rightP-tip)/max;

    Point tipDir = vec1.cross(vec2).normal();

    Point vecHeight = tip-center;
    Point vecBaseLeft = leftP-center;
    Point lefttangVec = vecHeight.cross(vecBaseLeft);
    Point leftT2 = leftP-tip;
    Point leftDir = center == tip? tipDir: leftT2.cross(lefttangVec).normal();

    Point vecBaseRight = rightP-center;
    Point righttangVec = vecHeight.cross(vecBaseRight);
    Point rightT2 = rightP-tip;
    Point rightDir = center == tip? tipDir: rightT2.cross(righttangVec).normal();

    pushPoint(shapePoints,tip);
    sp? pushPoint(shapePoints,tip.normal()): pushPoint(shapePoints,tipDir);
    if(!first){
        pushPoint(shapePoints,tip);
        sp? pushPoint(shapePoints,tip.normal()): pushPoint(shapePoints,tipDir);
        pushPoint(shapePoints,tip);
        sp? pushPoint(shapePoints,tip.normal()): pushPoint(shapePoints,tipDir);
    }

    for (int i=1; i<=max; i++){
        Point p1 = tip + vec1*i;
        Point p2 = tip + vec2*i;
        pushPoint(shapePoints,p1);
        sp? pushPoint(shapePoints,p1.normal()): pushPoint(shapePoints,leftDir);
        pushPoint(shapePoints,p2);
        sp? pushPoint(shapePoints,p2.normal()): pushPoint(shapePoints,rightDir);
        if(i==max){
            pushPoint(shapePoints,p2);
            sp? pushPoint(shapePoints,p2.normal()): pushPoint(shapePoints,rightDir);
        }
    }

}


void Shape::initCirc(Point tip, Point center, float r, bool clockwise, bool first, bool sp){
    Point p1(-r, center.y, 0);
    float degree = 360./param2;
    bool ff = false;

    for (int i = 0; i < param2; i++){
        if(i==0 && first){ff=true;}
        else {ff = false;}

        if(clockwise){
            initTri(tip, p1, p1.rotate(degree), center, ff, sp);
            p1 = p1.rotate(degree);
        }
        else{
            initTri(tip, p1, p1.rotate(-degree), center, ff, sp);
            p1 = p1.rotate(-degree);
        }
    }


}


void Shape::initcircSquare(Point startP, Point v1P, Point endP, Point center){
    Point vec1 = (v1P-startP)/param1;
    Point vec2 = endP-v1P;

    Point nor1 = startP-center;
    nor1.y=0;
    nor1 = nor1.normal();
    Point nor2 = endP-center;
    nor2.y=0;
    nor2 = nor2.normal();


    for (int i=0; i<=param1; i++){
        Point p1 = startP + vec1*i;
        Point p2 = startP + vec2 + vec1*i;
        if(i==0){
            pushPoint(shapePoints,p1);
            pushPoint(shapePoints,nor1);
        }
        pushPoint(shapePoints,p1);
        pushPoint(shapePoints,nor1);
        pushPoint(shapePoints,p2);
        pushPoint(shapePoints,nor2);
        if(i==param1){
            pushPoint(shapePoints,p2);
            pushPoint(shapePoints,nor2);
        }
    }
}

void Shape::initcircSquareSP(std::vector<Point> points){
    float degree = 360./param2;

    for (int i=0; i<=param2; i++){
        std::vector<Point> points2;
        for (int j = 0; j < points.size(); j++){
            points2.push_back(points[j].rotate(degree));
        }

        for (int i=0; i<points.size(); i++){
            if(i==0){
                pushPoint(shapePoints, points[i]);
                pushPoint(shapePoints, points[i].normal());
                pushPoint(shapePoints, points[i]);
                pushPoint(shapePoints, points[i].normal());
            }
            pushPoint(shapePoints, points[i]);
            pushPoint(shapePoints, points[i].normal());
            pushPoint(shapePoints, points2[i]);
            pushPoint(shapePoints, points2[i].normal());
            if(i==points.size()){
                pushPoint(shapePoints, points2[i]);
                pushPoint(shapePoints, points2[i].normal());
            }
        }

        points = points2;
    }



}

void Shape::pushPoint(std::vector<float> &a, Point b){
    a.push_back(b.x);
    a.push_back(b.y);
    a.push_back(b.z);
}

