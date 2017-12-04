#ifndef VIEW_H
#define VIEW_H

#include "GL/glew.h"
#include <qgl.h>
#include <QTime>
#include <QTimer>

#include <memory>
class Camera;
class OrbitingCamera;
class OpenGLScene;
class ShapesScene;
class SceneviewScene;


class View : public QGLWidget
{
    Q_OBJECT

public:
    View(QWidget *parent);
    ~View();

    // Returns a pointer to OrbitingCamera
    OrbitingCamera *getOrbitingCamera();

    // Returns a pointer to the current scene. If no scene is loaded, this function returns nullptr.
    OpenGLScene *getScene() { return m_currentScene; }

private:
    QTime m_time;
    QTimer m_timer;
    bool m_captureMouse;

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

private slots:
    void tick();


private:
    //We only have this camera
    std::unique_ptr<OrbitingCamera> m_defaultOrbitingCamera;
    bool m_isDragging;

    OpenGLScene *m_currentScene;
    std::unique_ptr<ShapesScene> m_shapesScene;
    std::unique_ptr<SceneviewScene> m_sceneviewScene;
    float holdTime;
};

#endif // VIEW_H
