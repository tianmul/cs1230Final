/**
 * @file   CamtransCamera.cpp
 *
 * This is the perspective camera class you will need to fill in for the Camtrans lab.  See the
 * lab handout for more details.
 */

#include "CamtransCamera.h"
//#include <Settings.h>

CamtransCamera::CamtransCamera()
{
    // @TODO: [CAMTRANS] Fill this in...
    near = 1.0;
    far = 30.0;
    setHeightAngle(60);
    setAspectRatio(1);
    eyepos = glm::vec3(2,2,2);
    lookat = glm::vec3(-2,-2,-2);
    up = glm::vec3(0,1,0);

    w= -glm::normalize(lookat);
    v= glm::normalize(up-glm::dot(up,w)*w);
    u= glm::cross(v,w);

}

void CamtransCamera::setAspectRatio(float a)
{
    // @TODO: [CAMTRANS] Fill this in...
    m_aspectRatio = a;

}

glm::mat4x4 CamtransCamera::getProjectionMatrix() const {
    // @TODO: [CAMTRANS] Fill this in...
    return getPerspectiveMatrix()*getScaleMatrix();
}

glm::mat4x4 CamtransCamera::getViewMatrix() const {
    // @TODO: [CAMTRANS] Fill this in...
   return glm::transpose(glm::mat4x4(
                       u.x,u.y,u.z,0,
                       v.x,v.y,v.z,0,
                       w.x,w.y,w.z,0,
                       0,0,0,1))*glm::transpose(glm::mat4x4(
                                                    1,0,0,-eyepos.x,
                                                    0,1,0,-eyepos.y,
                                                    0,0,1,-eyepos.z,
                                                    0,0,0,1));
}

glm::mat4x4 CamtransCamera::getScaleMatrix() const {
    // @TODO: [CAMTRANS] Fill this in...
    float hdiv2 = far*glm::tan(glm::radians(hangle/2));
    float wdiv2 = hdiv2*m_aspectRatio;
    float tanwdiv2 = wdiv2/far;
    float wangdiv2 = glm::atan(tanwdiv2);
    float cotw = 1.0f/(glm::tan(wangdiv2));
    float coth = 1.0f/(glm::tan(glm::radians(hangle/2)));


    //float wangle = glm::atan(glm::tan(glm::radians(hangle/2))*m_aspectRatio)*2;
    return glm::transpose(glm::mat4x4(
                       cotw/far, 0, 0, 0,
                       0, coth/far, 0, 0,
                       0, 0, 1.0f/far,0,
                       0,0,0,1));
}

glm::mat4x4 CamtransCamera::getPerspectiveMatrix() const {
    // @TODO: [CAMTRANS] Fill this in...
    float c = -near/far;
    return glm::transpose(glm::mat4x4(
                       1,0,0,0,
                       0,1,0,0,
                       0,0,-1.0f/(1.0f+c), c/(c+1.0f),
                       0,0,-1,0));
}

glm::vec4 CamtransCamera::getPosition() const {
    // @TODO: [CAMTRANS] Fill this in...
    return glm::vec4(eyepos,1);
}

glm::vec4 CamtransCamera::getLook() const {
    // @TODO: [CAMTRANS] Fill this in...
    return glm::vec4(glm::normalize(lookat),0);
}

glm::vec4 CamtransCamera::getUp() const {
    // @TODO: [CAMTRANS] Fill this in...
    return glm::vec4(glm::normalize(up),0);
}

float CamtransCamera::getAspectRatio() const {
    // @TODO: [CAMTRANS] Fill this in...
    return m_aspectRatio;
}

float CamtransCamera::getHeightAngle() const {
    // @TODO: [CAMTRANS] Fill this in...
    return hangle;
}

void CamtransCamera::orientLook(const glm::vec4 &eye, const glm::vec4 &look, const glm::vec4 &up) {
    // @TODO: [CAMTRANS] Fill this in...
    eyepos = eye.xyz();
    lookat = look.xyz();
    this->up = up.xyz();
    w= -glm::normalize(lookat);
    v= glm::normalize(this->up-glm::dot(this->up,w)*w);
    u= glm::cross(v,w);
    this->up = v;

}

void CamtransCamera::setHeightAngle(float h) {
    // @TODO: [CAMTRANS] Fill this in...
    hangle = h;
}

void CamtransCamera::translate(const glm::vec4 &v) {
    // @TODO: [CAMTRANS] Fill this in...
    eyepos = eyepos+v.xyz();

}

void CamtransCamera::rotateU(float degrees) {
    // @TODO: [CAMTRANS] Fill this in...
    glm::vec3 v0 = w*glm::sin(glm::radians(degrees)) + v*glm::cos(glm::radians(degrees));
    glm::vec3 w0 = -v*glm::sin(glm::radians(degrees)) + w*glm::cos(glm::radians(degrees));
    v=v0;
    w=w0;
    lookat = -w;
    up = v;
}

void CamtransCamera::rotateV(float degrees) {
    // @TODO: [CAMTRANS] Fill this in...
    glm::vec3 u0 = -w*glm::sin(glm::radians(degrees)) + u*glm::cos(glm::radians(degrees));
    glm::vec3 w0 = u*glm::sin(glm::radians(degrees)) + w*glm::cos(glm::radians(degrees));
    u=u0;
    w=w0;
    lookat = -w;

}

void CamtransCamera::rotateW(float degrees) {
    // @TODO: [CAMTRANS] Fill this in...
    glm::vec3 u0 = -v*glm::sin(-glm::radians(degrees)) + u*glm::cos(-glm::radians(degrees));
    glm::vec3 v0 = u*glm::sin(-glm::radians(degrees)) + v*glm::cos(-glm::radians(degrees));
    u=u0;
    v=v0;
    up = v;

}

void CamtransCamera::setClip(float nearPlane, float farPlane) {
    // @TODO: [CAMTRANS] Fill this in...
    near = nearPlane;
    far = farPlane;

}

