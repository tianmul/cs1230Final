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
    loadTexture("/home/jqian7/Documents/Final/cs1230Final/final/images/back.jpg", sk_back);
    loadTexture("/home/jqian7/Documents/Final/cs1230Final/final/images/front.jpg", sk_front);
    loadTexture("/home/jqian7/Documents/Final/cs1230Final/final/images/left.jpg", sk_left);
    loadTexture("/home/jqian7/Documents/Final/cs1230Final/final/images/right.jpg", sk_right);
    loadTexture("/home/jqian7/Documents/Final/cs1230Final/final/images/top.jpg", sk_top);

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
