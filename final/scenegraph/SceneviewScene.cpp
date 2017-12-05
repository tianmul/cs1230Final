#include "SceneviewScene.h"
#include "GL/glew.h"
#include <QGLWidget>
#include "camera/OrbitingCamera.h"

#include "lib/ResourceLoader.h"
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

#include "view.h"
using namespace CS123::GL;

#define P1 60
#define P2 60
#define P3 0
#define Lighting true


SceneviewScene::SceneviewScene()
{
    // TODO: [SCENEVIEW] Set up anything you need for your Sceneview scene here...
    loadPhongShader();
    /*loadWireframeShader();
    loadNormalsShader();
    loadNormalsArrowShader();*/

    // shadow map
    initShadowMap();
    glEnable(GL_DEPTH_TEST);
}

SceneviewScene::~SceneviewScene()
{
}

void SceneviewScene::loadPhongShader() {
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/default.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/default.frag");
    m_phongShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);
}
/*
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
*/

void SceneviewScene::initShadowMap() {
    // load depth shader
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/depth.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/depth.frag");
    m_depthShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);

    glGenFramebuffers(1, &depthMapFBO);
    glGenTextures(1, &depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}


void SceneviewScene::render(View *context) {
    setClearColor();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    m_depthShader->bind();
    m_depthShader->setLight(*m_Lights[0]);

    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glClear(GL_DEPTH_BUFFER_BIT);

    GLfloat near_plane = 0.1f, far_plane = 20.0f;
    glm::mat4 lightProjection = glm::ortho(-10.0f, 10.0f, -10.0f, 10.0f, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(glm::vec3(10.0f, 10.0f, 10.0f), glm::vec3(0.0f),  glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    m_depthShader->setUniform("lightSpaceMatrix",lightSpaceMatrix);

    glCullFace(GL_FRONT);
    renderGeometry(m_depthShader.get());
    glCullFace(GL_BACK);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    m_depthShader->unbind();

    // reset viewport
    glViewport(0, 0, context->width(), context->height());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


    m_phongShader->bind();
    setSceneUniforms(context);
    setMatrixUniforms(m_phongShader.get(), context);
    setLights();

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glUniform1i(glGetUniformLocation(m_phongShader->getID(), "depthMap"),1);

    m_phongShader->setUniform("lightSpaceMatrix",lightSpaceMatrix);

    renderGeometry(m_phongShader.get());
    glBindTexture(GL_TEXTURE_2D, 0);
    m_phongShader->unbind();

}

void SceneviewScene::setSceneUniforms(View *context) {
    OrbitingCamera *camera = context->getOrbitingCamera();
    m_phongShader->setUniform("useLighting", Lighting);
    m_phongShader->setUniform("useArrowOffsets", false);
    m_phongShader->setUniform("p" , camera->getProjectionMatrix());
    m_phongShader->setUniform("v", camera->getViewMatrix());
}

void SceneviewScene::setMatrixUniforms(Shader *shader, View *context) {
    shader->setUniform("p", context->getOrbitingCamera()->getProjectionMatrix());
    shader->setUniform("v", context->getOrbitingCamera()->getViewMatrix());
    shader->setUniform("m", glm::mat4(1.0f));
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
        m_phongShader->setLight(curlight);
    }
}

void SceneviewScene::renderGeometry(CS123Shader *shader) {
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    // TODO: [SCENEVIEW] Fill this in...
    // You shouldn't need to write *any* OpenGL in this class!
    //
    //
    // This is where you should render the geometry of the scene. Use what you
    // know about OpenGL and leverage your Shapes classes to get the job done.
    //


    for (int i = 0; i<m_shapeA.size(); i++){
        shader->setUniform("m", m_Primitives[marker[i]].second);
        CS123SceneMaterial mat = m_Primitives[marker[i]].first->material;
        mat.cDiffuse*=m_global.kd;
        mat.cAmbient*=m_global.ka;
        mat.cSpecular*=m_global.ks;
        mat.cTransparent*=m_global.kt;
        shader->applyMaterial(mat);
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
            m_shapeA.push_back(0);
            marker.push_back(i);
            break;
        case PrimitiveType::PRIMITIVE_CONE:
            m_shapeA.push_back(1);
            marker.push_back(i);
            break;
        case PrimitiveType::PRIMITIVE_CYLINDER:
            m_shapeA.push_back(2);
            marker.push_back(i);
            break;
        case PrimitiveType::PRIMITIVE_SPHERE:
            m_shapeA.push_back(3);
            marker.push_back(i);
            break;
        default:
            break;
        }
    }

    int p1,p2;
    p1 = P1;
    p2 = P2;
    float p3 = P3;

    m_shape.clear();
    m_shape.push_back(std::make_unique<Cube>());
    m_shape.push_back(std::make_unique<Cone>());
    m_shape.push_back(std::make_unique<Cylinder>());
    m_shape.push_back(std::make_unique<Sphere>());

    for (int i = 0; i<4; i++){
        m_shape[i]->setting(p1, p2, p3);
        m_shape[i]->init();
    }
}


