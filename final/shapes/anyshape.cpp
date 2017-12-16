#include "anyshape.h"


AnyShape::AnyShape() :
    OpenGLShape()
{
}

AnyShape::AnyShape(std::vector<float> vertex, bool withTexture):
    OpenGLShape()
{
    if(withTexture){
        m_vertex = vertex;
        sendToGLWithTexture(m_vertex);
    }
}


AnyShape::AnyShape(std::vector<float> vertex) :
    OpenGLShape()
{
    m_vertex = vertex;
    sendToGL(m_vertex);
}


AnyShape::~AnyShape()
{
}
