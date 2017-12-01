#ifndef RAYSCENE_H
#define RAYSCENE_H

#include "Scene.h"

#include <vector>
#include "ui/Canvas2D.h"
#include <unordered_map>


/**
 * @class RayScene
 *
 *  Students will implement this class as necessary in the Ray project.
 */
class RayScene : public Scene {
public:
    RayScene(Scene &scene);
    void render(Canvas2D* canvas, Camera* camera, int width, int height);
    void cancelRender();
    virtual ~RayScene();

private:
    bool intersectCube(glm::vec4 p_eye, glm::vec4 d, float &t, glm::vec4 &n);
    bool intersectSphere(glm::vec4 p_eye, glm::vec4 d, float &t, glm::vec4 &n);
    bool intersectCone(glm::vec4 p_eye, glm::vec4 d, float &t, glm::vec4 &n);
    bool intersectCylinder(glm::vec4 p_eye, glm::vec4 d, float &t, glm::vec4 &n);

    glm::vec2 uvCube(glm::vec4 p, glm::vec4 n);
    glm::vec2 uvSphere(glm::vec4 p, glm::vec4 n);
    glm::vec2 uvCone(glm::vec4 p, glm::vec4 n);
    glm::vec2 uvCylinder(glm::vec4 p, glm::vec4 n);

    glm::vec3 renderRecursive(glm::vec4 p_eye, glm::vec4 d_world, int recLevel);
    //glm::vec3 calculateColor(int bestIndex, glm::vec4 intersect, glm::vec4 n_world, glm::vec2 uv);
    glm::vec3 calculateColor(int bestIndex, glm::vec4 intersect, glm::vec4 n_world, glm::vec2 uv, glm::vec4 d_world);
    glm::vec2 calculateUV(int bestIndex, glm::vec4 intersect, glm::vec4 n_local);
    glm::vec3 textureColor(int bestIndex, glm::vec2 uv);
    bool findIntersect(glm::vec4 p_eye, glm::vec4 d_world, float &best_t, glm::vec4 &n, int &bestIndex);


    std::unordered_map<std::string, QImage> imagemap;
};

#endif // RAYSCENE_H
