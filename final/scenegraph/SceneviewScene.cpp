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
#include "gl/util/FullScreenQuad.h"


#include "glm/ext.hpp"
#include <iostream>
#include <sstream>
#include "gl/GLDebug.h"

#include "view.h"
using namespace CS123::GL;

#define P1 40
#define P2 30
#define P3 0
#define Lighting true


SceneviewScene::SceneviewScene()
    : init_done(false), usingShadowMap(true), usingSSAO(true)
{
    // TODO: [SCENEVIEW] Set up anything you need for your Sceneview scene here...
    /*loadWireframeShader();
    loadNormalsShader();
    loadNormalsArrowShader();*/

    // shadow map
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glEnable(GL_DEPTH_TEST);

    loadPhongShader();
//    initShadowMap();
    initSSAO();
    //glEnable(GL_MULTISAMPLE);
}

SceneviewScene::~SceneviewScene()
{
}
void SceneviewScene::reinit(){
    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    loadPhongShader();
    initSSAO();

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
    if (init_done) return;
    // load depth shader
    std::string vertexSource = ResourceLoader::loadResourceFileToString(":/shaders/depth.vert");
    std::string fragmentSource = ResourceLoader::loadResourceFileToString(":/shaders/depth.frag");
    m_depthShader = std::make_unique<CS123Shader>(vertexSource, fragmentSource);

    for (int i = 0; i < m_Lights.size(); ++i) {
        unsigned tmpFBO;
        unsigned tmpMap;
        glGenFramebuffers(1, &tmpFBO);
        glGenTextures(1, &tmpMap);
        glBindTexture(GL_TEXTURE_2D, tmpMap);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        GLfloat borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
        glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
        // attach depth texture as FBO's depth buffer
        glBindFramebuffer(GL_FRAMEBUFFER, tmpFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, tmpMap, 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        depthMapFBO.emplace_back(tmpFBO);
        depthMap.emplace_back(tmpMap);
    }

    init_done = true;
}

void SceneviewScene::toggleShadowMap() {
    usingShadowMap = !usingShadowMap;
}

void SceneviewScene::toggleSSAO(){
    usingSSAO = !usingSSAO;
}


void SceneviewScene::initSSAO(){
    // Setup SSAO shaders
    std::string vertexSource1 = ResourceLoader::loadResourceFileToString(":/shaders/ssao_geometry.vert");
    std::string fragmentSource1 = ResourceLoader::loadResourceFileToString(":/shaders/ssao_geometry.frag");
    SSAO_geometry = std::make_unique<CS123Shader>(vertexSource1, fragmentSource1);

    /*std::string vertexSource2 = ResourceLoader::loadResourceFileToString(":/shaders/ssao.vert");
    std::string fragmentSource2 = ResourceLoader::loadResourceFileToString(":/shaders/ssao_lighting.frag");
    SSAO_lighting = std::make_unique<CS123Shader>(vertexSource2, fragmentSource2);*/

    std::string vertexSource3 = ResourceLoader::loadResourceFileToString(":/shaders/ssao.vert");
    std::string fragmentSource3 = ResourceLoader::loadResourceFileToString(":/shaders/ssao.frag");
    SSAO = std::make_unique<CS123Shader>(vertexSource3, fragmentSource3);

    std::string vertexSource4 = ResourceLoader::loadResourceFileToString(":/shaders/ssao.vert");
    std::string fragmentSource4 = ResourceLoader::loadResourceFileToString(":/shaders/ssao_blur.frag");
    SSAO_blur = std::make_unique<CS123Shader>(vertexSource4, fragmentSource4);


/*change*/

    m_phongShader->bind();
    glUniform1i(glGetUniformLocation(m_phongShader->getID(), "gPositionDepth"), 2);
    glUniform1i(glGetUniformLocation(m_phongShader->getID(), "gNormal"), 3);
    glUniform1i(glGetUniformLocation(m_phongShader->getID(), "gAlbedo"), 4);
    glUniform1i(glGetUniformLocation(m_phongShader->getID(), "ssao"), 5);
    m_phongShader->unbind();

    SSAO->bind();
    glUniform1i(glGetUniformLocation(SSAO->getID(), "gPositionDepth"), 0);
    glUniform1i(glGetUniformLocation(SSAO->getID(), "gNormal"), 1);
    glUniform1i(glGetUniformLocation(SSAO->getID(), "texNoise"), 2);
    SSAO->unbind();


/*change end*/



    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);

    // - Position + linear depth color buffer
    glGenTextures(1, &gPositionDepth);
    glBindTexture(GL_TEXTURE_2D, gPositionDepth);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPositionDepth, 0);
    // - Normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    // - Albedo color buffer
    glGenTextures(1, &gAlbedo);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);

    glDrawBuffers(3, attachments);

    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "GBuffer Framebuffer not complete!" << std::endl;

    glGenFramebuffers(1, &ssaoFBO);  glGenFramebuffers(1, &ssaoBlurFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);

    glGenTextures(1, &ssaoColorBuffer);
       glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
       glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
       glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBuffer, 0);
       if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
           std::cout << "SSAO Framebuffer not complete!" << std::endl;
       // - and blur stage
       glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
       glGenTextures(1, &ssaoColorBufferBlur);
       glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);
       glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_FLOAT, NULL);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
       glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
       glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ssaoColorBufferBlur, 0);
       if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
           std::cout << "SSAO Blur Framebuffer not complete!" << std::endl;
       glBindFramebuffer(GL_FRAMEBUFFER, 0);

       randomFloats = std::uniform_real_distribution<float>(0.0f,1.0f);
       // Sample kernel
          for (GLuint i = 0; i < 64; ++i)
          {
              glm::vec3 sample(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, randomFloats(generator));
              sample = glm::normalize(sample);
              sample *= randomFloats(generator);
              GLfloat scale = GLfloat(i) / 64.0;

              // Scale samples s.t. they're more aligned to center of kernel
              scale = 0.1f + scale * scale * (1.0f - 0.1f);
              sample *= scale;
              ssaoKernel.push_back(sample);
          }

          // Noise texture
          for (GLuint i = 0; i < 16; i++)
          {
              glm::vec3 noise(randomFloats(generator) * 2.0 - 1.0, randomFloats(generator) * 2.0 - 1.0, 0.0f); // rotate around z-axis (in tangent space)
              ssaoNoise.push_back(noise);
          }
          glGenTextures(1, &noiseTexture);
          glBindTexture(GL_TEXTURE_2D, noiseTexture);
          glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, 4, 4, 0, GL_RGB, GL_FLOAT, &ssaoNoise[0]);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
          glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


          setClearColor();
}

void SceneviewScene::render(View *context) {
    initShadowMap(); // run only once

    setClearColor();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (usingShadowMap) {
        m_depthShader->bind();
        setLights(m_depthShader.get());

        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        for (int i = 0 ; i < m_Lights.size(); ++i) {
            glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO[i]);
            glClear(GL_DEPTH_BUFFER_BIT);

            m_depthShader->setUniform("lightIndex", i);
            glCullFace(GL_FRONT);
            renderGeometry(m_depthShader.get());
            glCullFace(GL_BACK);
    //        glBindTexture(GL_TEXTURE_2D, gNormal);
    //        glActiveTexture(GL_TEXTURE4);
    //        glBindTexture(GL_TEXTURE_2D, gAlbedo);

            glBindFramebuffer(GL_FRAMEBUFFER, 0);
        }

        m_depthShader->unbind();
    }


    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

    if (usingSSAO) {
        //SSAO
        // 1. Geometry Pass: render scene's geometry/color data into gbuffer
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        SSAO_geometry->bind();
        //setSceneUniforms(context);
        glm::mat4 projection = context->getOrbitingCamera()->getProjectionMatrix();
        glm::mat4 view = context->getOrbitingCamera()->getViewMatrix();
        //glm::mat4 model;
        glUniformMatrix4fv(glGetUniformLocation(SSAO_geometry->getID(), "p"), 1, GL_FALSE, glm::value_ptr(projection));
        glUniformMatrix4fv(glGetUniformLocation(SSAO_geometry->getID(), "v"), 1, GL_FALSE, glm::value_ptr(view));
        renderGeometry(SSAO_geometry.get());
        SSAO_geometry->unbind();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. Create SSAO texture
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        SSAO->bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPositionDepth);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, noiseTexture);

        for (GLuint i = 0; i < 64; ++i)
                        glUniform3fv(glGetUniformLocation(SSAO->getID(), ("samples[" + std::to_string(i) + "]").c_str()), 1, &ssaoKernel[i][0]);
        glUniformMatrix4fv(glGetUniformLocation(SSAO->getID(), "projection"), 1, GL_FALSE, glm::value_ptr(context->getOrbitingCamera()->getProjectionMatrix()));
        SSAO->setUniform("SCR_SIZE", glm::vec2(SCR_WIDTH, SCR_HEIGHT));

        std::unique_ptr<FullScreenQuad> t = std::make_unique<FullScreenQuad>();
        //glm::mat4 cammatrix = glm::inverse(glm::translate(glm::vec3(0.f, 0.f, 1.001f))*context->getOrbitingCamera()->getViewMatrix());
        t->draw();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        SSAO->unbind();


        // 3. Blur SSAO texture to remove noise
        glBindFramebuffer(GL_FRAMEBUFFER, ssaoBlurFBO);
        glClear(GL_COLOR_BUFFER_BIT);
        SSAO_blur->bind();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, ssaoColorBuffer);
        t->draw();
        SSAO_blur->unbind();
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }


    // 4. Lighting Pass: traditional deferred Blinn-Phong lighting now with added screen-space ambient occlusion
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_phongShader->bind();

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, gPositionDepth);
    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glActiveTexture(GL_TEXTURE4);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glActiveTexture(GL_TEXTURE5); // Add extra SSAO texture to lighting pass
    glBindTexture(GL_TEXTURE_2D, ssaoColorBufferBlur);

    setSceneUniforms(context);
    setMatrixUniforms(m_phongShader.get(), context);
    setLights(m_phongShader.get());

    // bind shadow map
    if (usingShadowMap) {
        for (int i = 0; i < m_Lights.size(); ++i) {
            glActiveTexture(GL_TEXTURE6+i);
            glBindTexture(GL_TEXTURE_2D, depthMap[i]);
            m_phongShader->setUniformArrayByIndex("depthMap", i+6, i);
        }
    }
    m_phongShader->setUniform("usingShadowMap", usingShadowMap);
    m_phongShader->setUniform("usingSSAO", usingSSAO);

    glUniform1i(glGetUniformLocation(m_phongShader->getID(), "square"), 0);
    m_phongShader->setUniform("SCR_SIZE", glm::vec2(SCR_WIDTH, SCR_HEIGHT));
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

void SceneviewScene::setLights(CS123::GL::CS123Shader* shader)
{
    // TODO: [SCENEVIEW] Fill this in...
    //
    // Set up the lighting for your scene using m_phongShader.
    // The lighting information will most likely be stored in CS123SceneLightData structures.
    //

    /*clear light*/
//    for (int i = 0; i < MAX_NUM_LIGHTS; i++) {
//        std::ostringstream os;
//        os << i;
//        std::string indexString = "[" + os.str() + "]"; // e.g. [0], [1], etc.
//        m_phongShader->setUniform("lightColors" + indexString, glm::vec3(0.0f, 0.0f, 0.0f));
//    }


    for (int i = 0; i<m_Lights.size(); i++){
        CS123SceneLightData curlight = *m_Lights[i];
        shader->setLight(curlight);
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


