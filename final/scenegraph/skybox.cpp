#include "skybox.h"

SkyBox::SkyBox()
{
    sk_back = 1;
    sk_front = 2;
    sk_left = 3;
    sk_right = 4;
    sk_top = 5;


}


void SkyBox::init()
{
    loadTexture(":/images/images/back.jpg", sk_back);
    loadTexture(":/images/images/front.jpg", sk_front);
    loadTexture(":/images/images/left.jpg", sk_left);
    loadTexture(":/images/images/right.jpg", sk_right);
    loadTexture(":/images/images/top.jpg", sk_top);

}


void SkyBox::loadTexture(QString fileName, GLuint &id)
{
    QImage image;
    image.load(fileName);
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}
