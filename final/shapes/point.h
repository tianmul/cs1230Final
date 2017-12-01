#ifndef POINT_H
#define POINT_H
#include "math.h"
#include <iostream>

#define PI 3.14159265

class Point
{
public:
    Point(float a=0, float b=0, float c=0){
        x=a;
        y=b;
        z=c;
    }

    Point normal(){
        Point rtval(0,0,0);
        float length = sqrt(this->x*this->x + this->y*this->y + this->z*this->z);
        if(length == 0) return rtval;
        else {
            rtval = *this/length;
        }
        return rtval;
    }

    Point operator+(const Point& another){
        Point rtval;
        rtval.x = this->x + another.x;
        rtval.y = this->y + another.y;
        rtval.z = this->z + another.z;
        return rtval;
    }


    Point operator-(const Point& another){
        Point rtval;
        rtval.x = this->x - another.x;
        rtval.y = this->y - another.y;
        rtval.z = this->z - another.z;
        return rtval;
    }

    Point operator*(const float& another){
        Point rtval;
        rtval.x = this->x * another;
        rtval.y = this->y * another;
        rtval.z = this->z * another;
        return rtval;
    }

    Point operator*(const int& another){
        Point rtval;
        rtval.x = this->x * another;
        rtval.y = this->y * another;
        rtval.z = this->z * another;
        return rtval;
    }

    Point operator/(const float& another){
        Point rtval;
        rtval.x = this->x / another;
        rtval.y = this->y / another;
        rtval.z = this->z / another;
        return rtval;
    }

    bool operator==(const Point& another){
        bool rtval = this->x == another.x && this->y == another.y && this->z == another.z;
        return rtval;
    }


    Point cross(const Point& another){
        Point rtval(0,0,0);
        rtval.x = this->y*another.z-this->z*another.y;
        rtval.y = this->z*another.x-this->x*another.z;
        rtval.z = this->x*another.y-this->y*another.x;
        return rtval;
    }

    Point rotate(const float& degree){
        Point rtval(0,0,0);
        rtval.x = cos(degree*PI/180)*this->x - sin(degree*PI/180)*this->z;
        rtval.y = this->y;
        rtval.z = sin(degree*PI/180)*this->x + cos(degree*PI/180)*this->z;
        return rtval;
    }
    void Printslash(){
        std::cout<<"-------"<<std::endl;
    }

    void Print(){
        std::cout<<this->x<<"||"<<this->y<<"||"<<this->z<<std::endl;
    }

    float x;
    float y;
    float z;
};
#endif // POINT_H
