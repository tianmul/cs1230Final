#ifndef SCENEVIEWSCENE_H
#define SCENEVIEWSCENE_H

#include "OpenGLScene.h"

#include <memory>
#include "shapes/shape.h"
#include "gl/textures/Texture2D.h"
#include <random>

namespace CS123 { namespace GL {

    class Shader;
    class CS123Shader;
}}

/**
 *
 * @class SceneviewScene
 *
 * A complex scene consisting of multiple objects. Students will implement this class in the
 * Sceneview assignment.
 *
 * Here you will implement your scene graph. The structure is up to you - feel free to create new
 * classes and data structures as you see fit. We are providing this SceneviewScene class for you
 * to use as a stencil if you like.
 *
 * Keep in mind that you'll also be rendering entire scenes in the next two assignments, Intersect
 * and Ray. The difference between this assignment and those that follow is here, we are using
 * OpenGL to do the rendering. In Intersect and Ray, you will be responsible for that.
 */
class SceneviewScene : public OpenGLScene {
public:
    SceneviewScene();
    void reinit();
    virtual ~SceneviewScene();

    virtual void render(View *context) override;
    virtual void settingsChanged() override;

    // Use this method to set an internal selection, based on the (x, y) position of the mouse
    // pointer.  This will be used during the "modeler" lab, so don't worry about it for now.
    void setSelection(int x, int y);
    unsigned int SCR_WIDTH = 800, SCR_HEIGHT = 600;

private:
    void loadPhongShader();
    /*
    void loadWireframeShader();
    void loadNormalsShader();
    void loadNormalsArrowShader();*/

    void setSceneUniforms(View *context);\
    void setMatrixUniforms(CS123::GL::Shader *shader, View *context);
    void setLights(CS123::GL::CS123Shader* shader);
    void renderGeometry(CS123::GL::CS123Shader* shader);

    std::unique_ptr<CS123::GL::CS123Shader> m_phongShader;
    /*std::unique_ptr<CS123::GL::Shader> m_wireframeShader;
    std::unique_ptr<CS123::GL::Shader> m_normalsShader;
    std::unique_ptr<CS123::GL::Shader> m_normalsArrowShader;*/


    std::vector<std::unique_ptr<Shape>> m_shape;
    std::vector<int> m_shapeA;
    std::vector<int> marker;

    // shadow map
    std::unique_ptr<CS123::GL::CS123Shader> m_depthShader;
    std::vector<Texture2D> m_depthTextures;
    std::vector<unsigned> depthMapFBO;
    std::vector<unsigned> depthMap;
    unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    void initShadowMap();
    bool init_done;

    // SSAO
    std::unique_ptr<CS123::GL::CS123Shader> SSAO_geometry;
    std::unique_ptr<CS123::GL::CS123Shader> SSAO;
    std::unique_ptr<CS123::GL::CS123Shader> SSAO_blur;
    std::unique_ptr<CS123::GL::CS123Shader> SSAO_lighting;

    unsigned int gBuffer;
    unsigned int gPositionDepth, gNormal, gAlbedo;
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    unsigned int rboDepth;
    unsigned int ssaoFBO, ssaoBlurFBO;
    unsigned int ssaoColorBuffer, ssaoColorBufferBlur;

    std::default_random_engine generator;
    std::vector<glm::vec3> ssaoKernel;
    std::vector<glm::vec3> ssaoNoise;
    std::uniform_real_distribution<float> randomFloats;
    GLuint noiseTexture;
    void initSSAO();

};

#endif // SCENEVIEWSCENE_H
