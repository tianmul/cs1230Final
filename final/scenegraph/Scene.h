#ifndef SCENE_H
#define SCENE_H

#include "lib/CS123SceneData.h"
#include <utility>
#include <vector>
#include <memory>
class Camera;
class CS123ISceneParser;


/**
 * @class Scene
 *
 * @brief This is the base class for all scenes. Modify this class if you want to provide
 * common functionality to all your scenes.
 */
class Scene {
public:
    Scene();
    Scene(Scene &scene);
    virtual ~Scene();

    virtual void settingsChanged() {}

    static void parse(Scene *sceneToFill, CS123ISceneParser *parser);


//protected:
    // Adds a primitive to the scene.
    virtual void addPrimitive(const CS123ScenePrimitive &scenePrimitive, const glm::mat4x4 &matrix);

    // Adds a light to the scene.
    virtual void addLight(const CS123SceneLightData &sceneLight);

    // Sets the global data for the scene.
    virtual void setGlobal(const CS123SceneGlobalData &global);

    virtual void treeToList(CS123SceneNode* treeRoot, glm::mat4x4 currentMatrix, std::vector<std::pair<CS123ScenePrimitive*, glm::mat4x4>> &vecPrimitive);

    CS123SceneGlobalData m_global;
    std::vector<std::pair<std::unique_ptr<CS123ScenePrimitive>, glm::mat4x4>> m_Primitives;
    std::vector<std::unique_ptr<CS123SceneLightData>> m_Lights;

};

#endif // SCENE_H
