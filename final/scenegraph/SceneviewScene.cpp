#include "SceneviewScene.h"
#include "GL/glew.h"
#include <QGLWidget>
#include "Camera.h"

#include "Settings.h"
#include "SupportCanvas3D.h"
#include "ResourceLoader.h"
#include "gl/shaders/CS123Shader.h"
#include <string>
#include <utility>
#include "shapes/cube.h"

#include "shapes/cone.h"

#include "shapes/sphere.h"

#include "shapes/cylinder.h"
#include "glm/ext.hpp"
#include <iostream>
#include <sstream>
#include "gl/GLDebug.h"
using namespace CS123::GL;


SceneviewScene::SceneviewScene()
{
    // TODO: [SCENEVIEW] Set up anything you need for your Sceneview scene here...
    loadPhongShader();
    loadWireframeShader();
    loadNormalsShader();
    loadNormalsArrowShader();

}

SceneviewScene::~SceneviewScene()
{
}

void SceneviewScene::loadPhongShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/default.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/default.frag");
    m_phongShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);
}

void SceneviewScene::loadWireframeShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/wireframe.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/wireframe.frag");
    m_wireframeShader = std::make_unique<Shader>(vertexSource, fragmentSource);
}

void SceneviewScene::loadNormalsShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/normals.vert");
    std::string geometrySource = ResourceLoader::loadResourceFileToString(":/shaders/normals.gsh");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/normals.frag");
    m_normalsShader = std::make_unique<Shader>(vertexSource, geometrySource, fragmentSource);
}

void SceneviewScene::loadNormalsArrowShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/normalsArrow.vert");
    std::string geometrySource = ResourceLoader::loadResourceFileToString(":/shaders/normalsArrow.gsh");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/normalsArrow.frag");
    m_normalsArrowShader = std::make_unique<Shader>(vertexSource, geometrySource, fragmentSource);
}

void SceneviewScene::render(SupportCanvas3D *context) {
    setClearColor();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_phongShader->bind();
    setSceneUniforms(context);
    setLights();
    renderGeometry();
    glBindTexture(GL_TEXTURE_2D, 0);
    m_phongShader->unbind();

}

void SceneviewScene::setSceneUniforms(SupportCanvas3D *context) {
    Camera *camera = context->getCamera();
    m_phongShader->setUniform("useLighting", settings.useLighting);
    m_phongShader->setUniform("useArrowOffsets", false);
    m_phongShader->setUniform("p" , camera->getProjectionMatrix());
    m_phongShader->setUniform("v", camera->getViewMatrix());
}

void SceneviewScene::setMatrixUniforms(Shader *shader, SupportCanvas3D *context) {
    shader->setUniform("p", context->getCamera()->getProjectionMatrix());
    shader->setUniform("v", context->getCamera()->getViewMatrix());
}

void SceneviewScene::setLights()
{
    // TODO: [SCENEVIEW] Fill this in...
    //
    // Set up the lighting for your scene using m_phongShader.
    // The lighting information will most likely be stored in CS123SceneLightData structures.
    //

    /*clear light*/
    for (int i = 0; i < MAX_NUM_LIGHTS; i++) {
        std::ostringstream os;
        os << i;
        std::string indexString = "[" + os.str() + "]"; // e.g. [0], [1], etc.
        m_phongShader->setUniform("lightColors" + indexString, glm::vec3(0.0f, 0.0f, 0.0f));
    }


    for (int i = 0; i<m_Lights.size(); i++){
        CS123SceneLightData curlight = *m_Lights[i];
        m_phongShader-> setLight(curlight);
    }
}

void SceneviewScene::renderGeometry() {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // TODO: [SCENEVIEW] Fill this in...
    // You shouldn't need to write *any* OpenGL in this class!
    //
    //
    // This is where you should render the geometry of the scene. Use what you
    // know about OpenGL and leverage your Shapes classes to get the job done.
    //


    for (int i = 0; i<m_shapeA.size(); i++){
        m_phongShader->setUniform("m", m_Primitives[marker[i]].second);
        CS123SceneMaterial mat = m_Primitives[marker[i]].first->material;
        mat.cDiffuse*=m_global.kd;
        mat.cAmbient*=m_global.ka;
        mat.cSpecular*=m_global.ks;
        mat.cTransparent*=m_global.kt;
        m_phongShader->applyMaterial(mat);
        m_shape[m_shapeA[i]]->draw();
    }

}

void SceneviewScene::settingsChanged() {
    // TODO: [SCENEVIEW] Fill this in if applicable.
    //std::cout << m_Primitives.size()<<std::endl;

    m_shapeA.clear();
    marker.clear();


    for (int i = 0; i<m_Primitives.size(); i++){
        switch (m_Primitives[i].first->type) {
        case PrimitiveType::PRIMITIVE_CUBE:
            //m_shape.push_back(std::make_unique<Cube>());
            m_shapeA.push_back(0);
            marker.push_back(i);
            break;
        case PrimitiveType::PRIMITIVE_CONE:
            //m_shape.push_back(std::make_unique<Cone>());
            m_shapeA.push_back(1);
            marker.push_back(i);
            break;
        case PrimitiveType::PRIMITIVE_CYLINDER:
            //m_shape.push_back(std::make_unique<Cylinder>());
            m_shapeA.push_back(2);
            marker.push_back(i);
            break;
        case PrimitiveType::PRIMITIVE_SPHERE:
            //m_shape.push_back(std::make_unique<Sphere>());
            m_shapeA.push_back(3);
            marker.push_back(i);
            break;
        default:
            break;
        }
    }


    int p1,p2;
    float p3;
    if (settings.shapeParameter1<25) p1 = 1;
    else if (settings.shapeParameter1 < 50) p1 = 8;
    else if (settings.shapeParameter1 < 75) p1 = 15;
    else if (settings.shapeParameter1 < 100) p1 = 22;
    else p1 = 29;

    if (settings.shapeParameter2<25) p2 = 4;
    else if (settings.shapeParameter2 < 50) p2 = 11;
    else if (settings.shapeParameter2 < 75) p2 = 18;
    else if (settings.shapeParameter2 < 100) p1 = 25;
    else p1 = 32;

    m_shape.clear();
    m_shape.push_back(std::make_unique<Cube>());
    m_shape.push_back(std::make_unique<Cone>());
    m_shape.push_back(std::make_unique<Cylinder>());
    m_shape.push_back(std::make_unique<Sphere>());

    for (int i = 0; i<4; i++){
        m_shape[i]->setting(p1, p2, settings.shapeParameter3);
        m_shape[i]->init();
    }
}


