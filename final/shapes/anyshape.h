#ifndef ANYSHAPE_H
#define ANYSHAPE_H

#include "shapes/openglshape.h"

class AnyShape : public OpenGLShape
{
public:
    AnyShape();
    ~AnyShape();
    AnyShape(std::vector<float> vertex);
    AnyShape(std::vector<float> vertex, bool withTexture);
protected:
    std::vector<float> m_vertex;
};

#endif // ANYSHAPE_H
