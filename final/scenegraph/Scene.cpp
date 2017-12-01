#include "Scene.h"
#include "camera/Camera.h"
#include "lib/CS123ISceneParser.h"
#include "lib/CS123SceneData.h"

#include "glm/gtx/transform.hpp"
#include "glm/ext.hpp"
#define PI 3.141592653571828
#include <iostream>

Scene::Scene()
{
}

Scene::Scene(Scene &scene)
{
    // We need to set the global constants to one when we duplicate a scene,
    // otherwise the global constants will be double counted (squared)

    CS123SceneGlobalData global = { 1, 1, 1, 1};
    setGlobal(global);

    // TODO [INTERSECT]
    // Make sure to copy over the lights and the scenegraph from the old scene,
    // as well as any other member variables your new scene will need.

}

Scene::~Scene()
{
    // Do not delete m_camera, it is owned by SupportCanvas3D
}

void Scene::parse(Scene *sceneToFill, CS123ISceneParser *parser) {
    //assert (m_Primitives.size() > 20);
    // TODO: load scene into sceneToFill using setGlobal(), addLight(), addPrimitive(), and
    //   finishParsing()
    CS123SceneGlobalData data;
    parser->getGlobalData(data);
    sceneToFill->setGlobal(data);

    sceneToFill->m_Primitives.clear();
    sceneToFill->m_Lights.clear();


    int numLight = parser->getNumLights();
    for (int i = 0; i<numLight; i++){
        CS123SceneLightData lightData;
        parser->getLightData(i, lightData);
        sceneToFill->addLight(lightData);
    }

    CS123SceneNode* treeRoot = parser->getRootNode();
    std::vector<std::pair<CS123ScenePrimitive*, glm::mat4x4>> vecPrimitive;
    sceneToFill->treeToList(treeRoot, glm::mat4x4() ,vecPrimitive);

    for (int i = 0; i < vecPrimitive.size(); i++){
        std::pair<CS123ScenePrimitive*, glm::mat4x4> curelem = vecPrimitive[i];
        sceneToFill->addPrimitive(*(curelem.first), curelem.second);
    }


}

void Scene::treeToList(CS123SceneNode* treeRoot, glm::mat4x4 currentMatrix ,std::vector<std::pair<CS123ScenePrimitive*, glm::mat4x4>> &vecPrimitive) {
    glm::mat4x4 trans = currentMatrix;

    for (int i = 0; i<treeRoot->transformations.size(); i++){

        CS123SceneTransformation *t = treeRoot->transformations[i];
        glm::vec3 v;
        float a,theta, phi;
        switch(t->type) {
        case TRANSFORMATION_TRANSLATE:
            v = t->translate;
            trans *= glm::mat4( 1,   0,   0,   0,
                                0,   1,   0,   0,
                                0,   0,   1,   0,
                                v.x,   v.y,   v.z,   1);
            break;
        case TRANSFORMATION_SCALE:
            v = t->scale;
            trans *= glm::mat4( v.x,   0,   0,   0,
                                0,   v.y,   0,   0,
                                0,   0,   v.z,   0,
                                0,   0,   0,   1);
            break;
        case TRANSFORMATION_ROTATE:
            v = t->rotate;
            a = t->angle;

            theta = atan2(v.z, v.x);

            phi = atan2(v.y, glm::distance(glm::vec2(), glm::vec2(v.x, v.z)));

            trans *= glm::mat4(cos(theta),   0,     sin(theta),    0,
                                   0,        1,         0,         0,
                              -sin(theta),   0,     cos(theta),    0,
                                   0,        0,         0,         1);

            trans *= glm::mat4(cos(phi), sin(phi),     0,         0,
                               -sin(phi),  cos(phi),     0,         0,
                                  0,         0,         1,         0,
                                  0,         0,         0,         1);

            trans *= glm::mat4(   1,         0,         0,         0,
                                  0,       cos(a),   sin(a),       0,
                                  0,      -sin(a),   cos(a),       0,
                                  0,         0,         0,         1);

            trans *= glm::mat4(cos(phi),  -sin(phi),     0,         0,
                              sin(phi),  cos(phi),     0,         0,
                                  0,         0,         1,         0,
                                  0,         0,         0,         1);

            trans *= glm::mat4(cos(theta),   0,   -sin(theta),     0,
                                  0,         1,         0,         0,
                              sin(theta),    0,    cos(theta),     0,
                                  0,         0,         0,         1);
            break;
        case TRANSFORMATION_MATRIX:
            trans *= t->matrix;
            break;
        default:
            break;
        }
    }
    for (int i = 0; i<treeRoot->primitives.size(); i++){
        vecPrimitive.push_back(std::make_pair(treeRoot->primitives[i], trans));
    }
    for (int i = 0; i<treeRoot->children.size(); i++){
        treeToList(treeRoot->children[i], trans, vecPrimitive);
    }

}

void Scene::addPrimitive(const CS123ScenePrimitive &scenePrimitive, const glm::mat4x4 &matrix) {
    std::unique_ptr<CS123ScenePrimitive> primitive = std::make_unique<CS123ScenePrimitive>();
    *primitive = scenePrimitive;
    m_Primitives.push_back(std::make_pair(std::move(primitive), matrix));
}

void Scene::addLight(const CS123SceneLightData &sceneLight) {
    std::unique_ptr<CS123SceneLightData> light = std::make_unique<CS123SceneLightData>();
    *light = sceneLight;
    m_Lights.push_back(std::move(light));
}

void Scene::setGlobal(const CS123SceneGlobalData &global) {
    m_global = global;
}

