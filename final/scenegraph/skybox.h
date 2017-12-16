#ifndef SKYBOX_H
#define SKYBOX_H

#include <QImage>
#include "shapes/openglshape.h"

class SkyBox
{
public:
    SkyBox();
    void init();
    GLuint sk_front, sk_left, sk_right,sk_top,sk_back;


private:
    enum {SKY_LEFT=0,SKY_BACK,SKY_RIGHT,SKY_FRONT,SKY_TOP,SKY_BOTTOM};
    void loadTexture(QString fileName, GLuint &id);
};

#endif // SKYBOX_H
