#include "view.h"

#include "viewformat.h"
#include <QApplication>
#include <QKeyEvent>
#include <QMessageBox>
#include <iostream>

#include "camera/OrbitingCamera.h"
#include "scenegraph/ShapesScene.h"
#include "scenegraph/SceneviewScene.h"
#include "lib/CS123XmlSceneParser.h"

//if we want to read file, change it to true
#define READFILE true
#define FILEPATH "/course/cs1230/data/scenes/ray/all_objects.xml"
#define CameraSpeed 15.0f

//edit the whole program and add all comments, Dec.2, by Lan


//Since we only use OrbitingCamera, this substitute the getCamera() in SupportCanvas3D()
OrbitingCamera *View::getOrbitingCamera(){
    return m_defaultOrbitingCamera.get();
}


/* all codes below are stencil code*/

View::View(QWidget *parent) : QGLWidget(ViewFormat(), parent),
    m_time(), m_timer(), m_captureMouse(false),m_defaultOrbitingCamera(new OrbitingCamera()),
    m_currentScene(nullptr), m_isDragging(false)
{
    // View needs all mouse move events, not just mouse drag events
    setMouseTracking(true);

    // Hide the cursor
    if(m_captureMouse) {
        QApplication::setOverrideCursor(Qt::BlankCursor);
    }

    // View needs keyboard focus
    setFocusPolicy(Qt::StrongFocus);

    // The update loop is implemented using a timer
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));
}

View::~View()
{
}



void View::initializeGL()
{
    // All OpenGL initialization *MUST* be done during or after this
    // method. Before this method is called, there is no active OpenGL
    // context and all OpenGL calls have no effect.

    //initialize glew
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if ( GLEW_OK != err ) {
        /* Problem: glewInit failed, something is seriously wrong. */
        std::cerr << "Something is very wrong, glew initialization failed." << std::endl;
    }
    std::cout << "Using GLEW " <<  glewGetString( GLEW_VERSION ) << std::endl;

    // Start a timer that will try to get 60 frames per second (the actual
    // frame rate depends on the operating system and other running programs)
    m_time.start();
    m_timer.start(1000 / 60);

//    glEnable(GL_DEPTH_TEST);
//    glEnable(GL_CULL_FACE);
//    glCullFace(GL_BACK);
//    glFrontFace(GL_CCW);


    //------------------by Lan---------------------
    //Updating the camera matrix,
    getOrbitingCamera()->updateMatrices();

    //Build the scene

    m_shapesScene = std::make_unique<ShapesScene>(width(), height());
    m_sceneviewScene = std::make_unique<SceneviewScene>();

    if(!READFILE) m_currentScene = m_shapesScene.get();
    else {
        m_currentScene = m_sceneviewScene.get();

        //Load scene from file
        QString file = FILEPATH;
        CS123XmlSceneParser parser(file.toLatin1().data());

        if (parser.parse()) {
            Scene::parse(m_sceneviewScene.get(), &parser);

            // Set the camera for the new scene
            CS123SceneCameraData camera;
            if (parser.getCameraData(camera)) {
                //Since we use Orbiting Camera here, just do nothing
            }
        } else {
            QMessageBox::critical(this, "Error", "Could not load scene \"" + file + "\"");
        }
    }

    //Refresh the screen to load the file to scene
    m_currentScene->settingsChanged();
}

void View::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // TODO: Implement the demo rendering here

    //set Camera and rendering
    m_currentScene->render(this);
}

void View::resizeGL(int w, int h)
{
    float ratio = static_cast<QGuiApplication *>(QCoreApplication::instance())->devicePixelRatio();
    w = static_cast<int>(w / ratio);
    h = static_cast<int>(h / ratio);
    glViewport(0, 0, w, h);


    //resize camera
     getOrbitingCamera()->setAspectRatio(static_cast<float>(width()) / static_cast<float>(height()));
}

void View::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::RightButton) {
        getOrbitingCamera()->mouseDown(event->x(), event->y());
        m_isDragging = true;
    }

}

void View::mouseMoveEvent(QMouseEvent *event)
{
    // This starter code implements mouse capture, which gives the change in
    // mouse position since the last mouse movement. The mouse needs to be
    // recentered after every movement because it might otherwise run into
    // the edge of the screen, which would stop the user from moving further
    // in that direction. Note that it is important to check that deltaX and
    // deltaY are not zero before recentering the mouse, otherwise there will
    // be an infinite loop of mouse move events.
    if(m_captureMouse) {
        int deltaX = event->x() - width() / 2;
        int deltaY = event->y() - height() / 2;
        if (!deltaX && !deltaY) return;
        QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));

        // TODO: Handle mouse movements here
    }


    if (m_isDragging) {
        getOrbitingCamera()->mouseDragged(event->x(), event->y());
    }
}

void View::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_isDragging && event->button() == Qt::RightButton) {
        getOrbitingCamera()->mouseUp(event->x(), event->y());
        m_isDragging = false;
    }
}

void View::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) QApplication::quit();

    float speed = CameraSpeed;
    float seconds = m_time.elapsed()* 0.001f;

    if (event->key() == Qt::Key_W) getOrbitingCamera()->mouseScrolled(0.f,speed*seconds);
    else if (event->key() == Qt::Key_S) getOrbitingCamera()->mouseScrolled(0.f,-speed*seconds);
    else if (event->key() == Qt::Key_A) getOrbitingCamera()->mouseScrolled(speed*seconds, 0.f);
    else if (event->key() == Qt::Key_D) getOrbitingCamera()->mouseScrolled(-speed*seconds, 0.f);

    // TODO: Handle keyboard presses here
}

void View::keyReleaseEvent(QKeyEvent *event)
{
}

void View::tick()
{
    // Get the number of seconds since the last tick (variable update rate)
    float seconds = m_time.restart() * 0.001f;

    // TODO: Implement the demo update here
    //at most 60 frams per second

    update();
    /*
    if(seconds > 1.0f/60.0f)update();

    if(seconds > 0.1f)std::cout << "fps too low:" << seconds << std::endl;*/

    // Flag this view for repainting (Qt will call paintGL() soon after)
    //update();
}
