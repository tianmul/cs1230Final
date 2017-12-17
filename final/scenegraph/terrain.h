#ifndef TERRAIN_H
#define TERRAIN_H

#define GLM_FORCE_RADIANS
#include "glm/glm.hpp"            // glm::vec*, mat*, and basic glm functions
#include "glm/gtx/transform.hpp"  // glm::translate, scale, rotate
#include "glm/gtc/type_ptr.hpp"   // glm::value_ptr
#include <vector>
#include <QImage>
#include "shapes/openglshape.h"

class Terrain {
public:
    Terrain();
    ~Terrain();
    void init();
    void draw();
    GLuint m_textureID;
    std::vector<glm::vec3> grassPos;
    std::vector<glm::vec3> grassPosNormal;

private:
    float randValue(int row, int col);
    glm::vec3 getPosition(int row, int col);
    glm::vec3 getNormal(int row, int col);
    glm::vec3 getUV(int row, int col);
    glm::vec3 getColor();
    std::unique_ptr<OpenGLShape> m_shape;
    float m_numRows, m_numCols;
    void loadHeightMap(QString filename);
    void convertHeightMap(QImage heightMap);
    bool seed(int s, int base);
    glm::vec3 getPositionFromHeightMap(int row, int col);
    std::vector<glm::vec3> heightMapList;
    QImage gHeightMap;
    int gWorldMapWidth;
    int gWorldMapHeight;
    GLuint m_terrainTexture;

    //GLuint m_textureID;

};

#endif // TERRAIN_H
