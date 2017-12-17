#include "terrain.h"

#include <math.h>
#include "gl/shaders/ShaderAttribLocations.h"
#include <QImage>
#include <iostream>

#include "lib/ResourceLoader.h"

Terrain::Terrain()
{

}

Terrain::~Terrain()
{
    // TODO: Clean up GPU memory.
    glDeleteTextures(1, &m_textureID);
}


/**
 * Returns a pseudo-random value between -1.0 and 1.0 for the given row and column.
 */
float Terrain::randValue(int row, int col) {
    return -1.0 + 2.0 * glm::fract(sin(row * 127.1f + col * 311.7f) * 43758.5453123f);
}


/* load height map*/
void Terrain::loadHeightMap(QString filename){
    gHeightMap.load(filename);
    m_numRows = gHeightMap.width();
    m_numCols = gHeightMap.height();
    gWorldMapWidth = gHeightMap.width();
    gWorldMapHeight = gHeightMap.height();
    //QImage heightmap = gHeightMap;
    //heightMapList = std::vector<glm::vec3>(heightMap.width() * heightMap.height());
    //convertHeightMap(heightMap);
}

void Terrain::convertHeightMap(QImage heightMap){
    int index = 0;
    for(int x =0 ; x < heightMap.width(); x ++){
        for(int y = 0; y < heightMap.height(); y ++){
            index = x * heightMap.width() + y;
            QRgb q = heightMap.pixel(x,y);
            heightMapList[index] = glm::vec3(qRed(q),qGreen(q),qBlue(q));
        }
    }
}


/**
 * Returns the object-space position for the terrain vertex at the given row and column.
 */
glm::vec3 Terrain::getPositionFromHeightMap(int row, int col){
    /* clamp */
    row = std::fmax(2,row);
    col = std::fmax(2,col);
    row = std::fmin(gWorldMapWidth-2,row);
    col = std::fmin(gWorldMapHeight-2,col);

     QRgb q = gHeightMap.pixel(row,col);
     glm::vec3 position;
     position.x = 10 * row/m_numRows - 5;
     position.y = qBlue(q) / 255.f - 0.5f;
  //   std::cout << qBlue(q) / 255.f <<std::endl;
     //position.y = randValue(row,col);
     position.z = 10 * col/m_numCols - 5;

     int nRow = row / 16.f;
     int nCol = col / 16.f;

     nRow = std::fmax(2,nRow);
     nCol = std::fmax(2,nCol);
     nRow = std::fmin(gWorldMapWidth-1,nRow);
     nCol = std::fmin(gWorldMapHeight-1,nCol);

     // starting point
     float C = qBlue(gHeightMap.pixel(nRow,nCol)) / 255.f - 0.5f;
     float D = qBlue(gHeightMap.pixel(nRow+1,nCol)) / 255.f- 0.5f;
     float B = qBlue(gHeightMap.pixel(nRow+1,nCol-1)) / 255.f- 0.5f;
     float A = qBlue(gHeightMap.pixel(nRow,nCol-1)) / 255.f- 0.5f;

     float x = glm::fract(row / 16.f);    //draw grass
     float y = glm::fract(col / 16.f);

     float val1 = glm::mix(A,B,x*x*(3-2*x));
     float val2 = y * glm::mix(C,D,x*x*(3-2*x));

     position.y += (1-y) *val1 + val2;

     nRow = row / 1.f;
     nCol = col / 1.f;

     nRow = std::fmax(2,nRow);
     nCol = std::fmax(2,nCol);
     nRow = std::fmin(gWorldMapWidth-1,nRow);
     nCol = std::fmin(gWorldMapHeight-1,nCol);


     // starting point
      C = qBlue(gHeightMap.pixel(nRow,nCol)) / 255.f - 0.5f;
      D = qBlue(gHeightMap.pixel(nRow+1,nCol)) / 255.f- 0.5f;
      B = qBlue(gHeightMap.pixel(nRow+1,nCol-1)) / 255.f- 0.5f;
      A = qBlue(gHeightMap.pixel(nRow,nCol-1)) / 255.f- 0.5f;

     x = glm::fract(row / 1.f);
     y = glm::fract(col / 1.f);

     position.y += 0.5f+ ((1-y) * glm::mix(A,B,x*x*(3-2*x)) + y * glm::mix(C,D,x*x*(3-2*x)));

     // TODO: Adjust position.y using value noise.
     //position.y = randValue(row,col);
     return position;

}

/**
 * Returns the object-space position for the terrain vertex at the given row and column.
 */
glm::vec3 Terrain::getPosition(int row, int col) {
    glm::vec3 position;
    position.x = 10 * row/m_numRows - 5;
    position.y = 0;
    position.z = 10 * col/m_numCols - 5;

    // TODO: Adjust position.y using value noise.
    position.y = randValue(row,col);

    int nRow = row / 32;
    int nCol = col / 32;

    // starting point
    float C = randValue(nRow,nCol);
    float D = randValue(nRow+1,nCol);
    float B = randValue(nRow+1,nCol-1);
    float A = randValue(nRow,nCol-1);

    float x = glm::fract(row / 32.f);
    float y = glm::fract(col / 32.f);

    // linear
    //position.y = (1-y) * glm::mix(A,B,x) + y * glm::mix(C,D,x);

    // bicubic
    position.y = (1-y) * glm::mix(A,B,x*x*(3-2*x)) + y * glm::mix(C,D,x*x*(3-2*x));

    nRow = row / 16;
    nCol = col / 16;

    // starting point
    C = randValue(nRow,nCol);
    D = randValue(nRow+1,nCol);
    B = randValue(nRow+1,nCol-1);
    A = randValue(nRow,nCol-1);

    x = glm::fract(row / 16.f);
    y = glm::fract(col / 16.f);

     position.y += 1*((1-y) * glm::mix(A,B,x*x*(3-2*x)) + y * glm::mix(C,D,x*x*(3-2*x)));



    nRow = row / 4;
    nCol = col / 4;

    // starting point
    C = randValue(nRow,nCol);
    D = randValue(nRow+1,nCol);
    B = randValue(nRow+1,nCol-1);
    A = randValue(nRow,nCol-1);

    x = glm::fract(row / 4.f);
    y = glm::fract(col / 4.f);

    position.y +=  0.2 * ((1-y) * glm::mix(A,B,x*x*(3-2*x)) + y * glm::mix(C,D,x*x*(3-2*x)));


    return position;
}

glm::vec3 Terrain::getColor(){
    return glm::vec3(0.5f,0.85f,0.35f);
}
/* returns the color and the uv of the current point */
glm::vec3 Terrain::getUV  (int row, int col){
    float u = row / m_numRows;
    float v = col / m_numCols;
    return glm::vec3(u,v,0);
}

/**
 * Returns the normal vector for the terrain vertex at the given row and column.
 */
glm::vec3 Terrain::getNormal(int row, int col) {
    // TODO: Compute the normal at the given row and column using the positions of the
    //       neighboring vertices.


    glm::vec3 n1 =  glm::cross(getPositionFromHeightMap(row+1,col),getPositionFromHeightMap(row+1,col-1));
    glm::vec3 n2 =  glm::cross(getPositionFromHeightMap(row+1,col-1),getPositionFromHeightMap(row,col-1));
    glm::vec3 n3 =  glm::cross(getPositionFromHeightMap(row,col-1),getPositionFromHeightMap(row-1,col-1));
    glm::vec3 n4 =  glm::cross(getPositionFromHeightMap(row-1,col-1),getPositionFromHeightMap(row-1,col));
    glm::vec3 n5 =  glm::cross(getPositionFromHeightMap(row-1,col),getPositionFromHeightMap(row-1,col+1));
    glm::vec3 n6 =  glm::cross(getPositionFromHeightMap(row-1,col+1),getPositionFromHeightMap(row,col+1));
    glm::vec3 n7 =  glm::cross(getPositionFromHeightMap(row,col+1),getPositionFromHeightMap(row+1,col+1));
    glm::vec3 n8 =  glm::cross(getPositionFromHeightMap(row+1,col+1),getPositionFromHeightMap(row+1,col));
    glm::vec3 f = (n1 + n2 + n3 + n4 + n5 + n6 + n7 + n8);
    f[0] = f[0] / 8;
    f[1] = f[1] / 8;
    f[2] = f[2] / 8;
    //return glm::vec3(0, 1, 0);
    return glm::normalize(f);
}


/**
 * Initializes the terrain by storing positions and normals in a vertex buffer.
 */
void Terrain::init() {

    // TODO: Change from GL_LINE to GL_FILL in order to render full triangles instead of wireframe.
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glShadeModel(GL_SMOOTH);
    loadHeightMap(":/images/images/depth.png");

    // TODO (Task 6): Initialize texture map.
    QImage image(":/images/images/map1.jpg");
    glGenTextures(1, &m_textureID);
    glBindTexture(GL_TEXTURE_2D, m_textureID);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.bits());
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // Initializes a grid of vertices using triangle strips.
    int numVertices = (m_numRows - 1) * (2 * m_numCols + 2);
    std::vector<glm::vec3> data(4 * numVertices);
    int index = 0;
    for (int row = 0; row < m_numRows - 1; row++) {
        for (int col = m_numCols - 1; col >= 0; col--) {
            if(col % 100 == 0){
            data[index++] = getPositionFromHeightMap(row, col);
            data[index++] = getNormal  (row, col);
            data[index++] = getColor();
            data[index++] = getUV  (row, col);
            data[index++] = getPositionFromHeightMap(row + 1, col);
            data[index++] = getNormal  (row + 1, col);
            data[index++] = getColor();
            data[index++] = getUV  (row, col);
            }
            // generate tree
            if(seed(2999999999)){
                glm::vec3 pos = getPositionFromHeightMap(row, col);
                glm::vec3 normal = getNormal  (row, col);
                float scalex = rand() % 100 / 1000.f;
                float scaley = rand() % 100 / 800.f;
                float scalez = rand() % 100 / 500.f;
                float sx = rand() % 100 / 200.f;
                float sy = rand() % 100 / 150.f;
                float sz = rand() % 100 / 100.f;
                std::cout<< "<transblock>"
                            "<translate x=\""<<pos[0]<<"\" y=\""<<(pos[1]+0.2f)<<"\" z=\""<<pos[2]<<"\" />"
                            "<scale x=\""<<0.1 + scalex <<"\" y=\""<<0.1 + scaley <<"\" z=\""<<0.1 + scalex <<"\" />"
                            "<object type=\"tree\">"
                            "<transblock>"
                            "<translate x=\"0\" y=\"-1.5\" z=\"0\" />"
                                "<scale x=\"0.3\" y=\"2.5\" z=\"0.3\" />"
                                "<object type=\"primitive\" name=\"cylinder\">"
                                    "<diffuse r=\"0.7\" g=\"0.2\" b=\"0\"/>"
                                    "<ambient r=\"0.4\" g=\"0.4\" b=\"0.4\"/>"
                                    "</object>"
                            "</transblock>"
                            "<transblock>"    //draw grass
                                "<translate x=\"0\" y=\"0.3\" z=\"-0.3\" />"
                                "<scale x=\""<<1.2 + sx <<"\" y=\""<<1.2 + sx <<"\" z=\""<<1.2 + sx <<"\" />"
                                "<object type=\"primitive\" name=\"sphere\">"
                                    "<diffuse r=\"0\" g=\"1\" b=\"0\"/>"
                                    "<ambient r=\"0.4\" g=\"0.4\" b=\"0.4\"/>"
                                "</object>"
                            "</transblock>"
                            "<transblock>"
                                "<translate x=\"0\" y=\"0.3\" z=\"0.3\" />"
                                "<scale x=\""<<1.2 + sx <<"\" y=\""<<1.2 + sx <<"\" z=\""<<1.2 + sx <<"\" />"
                                "<object type=\"primitive\" name=\"sphere\">"
                                    "<diffuse r=\"0\" g=\"1\" b=\"0\"/>"
                                    "<ambient r=\"0.4\" g=\"0.4\" b=\"0.4\"/>"
                                "</object>"
                            "</transblock>"
                            "<new/prefix1()transblock>"
                            "    <translate x=\": m_textureID(10)glBindTexture(GL_TEXTURE_2D, m_terrain.m_textureID);-0.3\" y=\"0.3\" z=\"0\" />"
                            "<scale x=\""<<1.2 + sy <<"\" y=\""<<1.2 + sy <<"\" z=\""<<1.2 + sy <<"\" />"
                            "   <object type=\"primitive\" name=\"sphere\">"
                            "        <diffuse r=\"0\" g=\"1\" b=\"0\"/>"
                                    "<ambient r=\"0.3\" g=\"0.3\" b=\"0.3\"/>"
                            "    </object>"
                            "</transblock>"
                            "<transblock>"
                                "<translate x=\"0.3\" y=\"0.3\" z=\"0\" />"
                               "<scale x=\""<<1.2 + sy <<"\" y=\""<<1.2 + sy <<"\" z=\""<<1.2 + sy <<"\" />"
                                "<object type=\"primitive\" name=\"sphere\">"
                                    "<diffuse r=\"0\" g=\"1\" b=\"0\"/>"
                                    "<ambient r=\"0.4\" g=\"0.4\" b=\"0.4\"/>"
                                "</object>"
                            "</transblock>"
                            "<transblock>"
                               "<translate x=\"0\" y=\"0.7\" z=\"0\" />"
                                "<scale x=\""<<1.2 + sz <<"\" y=\""<<1.2 + sz <<"\" z=\""<<1.2 + sz <<"\" />"
                                "<object type=\"primitive\" name=\"sphere\">"
                                    "<diffuse r=\"0\" g=\"1\" b=\"0\"/>"
                                    "<ambient r=\"0.4\" g=\"0.4\" b=\"0.4\"/>"
                                "</object>"
                            "</transblock>"
                        "</object>"
                    "</transblock>" << std::endl;
            }
        }
        data[index++] = getPositionFromHeightMap(row + 1, 0);
        data[index++] = getNormal  (row + 1, 0);
        data[index++] = getColor();
        data[index++] = getUV  (row, 0);
        data[index++] = getPositionFromHeightMap(row + 1, m_numCols - 1);
        data[index++] = getNormal  (row + 1, m_numCols - 1);
        data[index++] = getColor();
        data[index++] = getUV  (row, m_numCols - 1);
    }
    m_shape = std::make_unique<OpenGLShape>();
    m_shape->setVertexData(&data[0][0], data.size() * 3, VBO::GEOMETRY_LAYOUT::LAYOUT_TRIANGLE_STRIP, numVertices);
    m_shape->setAttribute(ShaderAttrib::POSITION, 3, 0, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_shape->setAttribute(ShaderAttrib::NORMAL, 3, 12, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_shape->setAttribute(ShaderAttrib::COLOR, 3, 24, VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_shape->setAttribute(ShaderAttrib::TEXCOORD0, 3, 36 , VBOAttribMarker::DATA_TYPE::FLOAT, false);
    m_shape->buildVAO();
}

bool Terrain::seed(int s){
    int max = rand() % s;

    if( max == 99)
         return true;
    return false;
}

/**
 * Draws the terrain.
 */
void Terrain::draw()
{
    m_shape->draw();
}
