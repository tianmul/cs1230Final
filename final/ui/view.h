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


class View : public QGLWidget
{
    Q_OBJECT

public:
    View(QWidget *parent);
    ~View();


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
    //currently suppose we only have this one
    std::unique_ptr<OrbitingCamera> m_defaultOrbitingCamera;
    bool m_isDragging;

    OpenGLScene *m_currentScene;
};

#endif // VIEW_H
