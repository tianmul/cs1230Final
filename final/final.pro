# -------------------------------------------------
# Project created by QtCreator 2010-08-22T14:12:19
# -------------------------------------------------
QT += opengl xml
TARGET = final
TEMPLATE = app

QMAKE_CXXFLAGS += -std=c++14
CONFIG += c++14

unix:!macx {
    LIBS += -lGLU
}
macx {
    QMAKE_CFLAGS_X86_64 += -mmacosx-version-min=10.7
    QMAKE_CXXFLAGS_X86_64 = $$QMAKE_CFLAGS_X86_64
    CONFIG += c++11
}
win32 {
    DEFINES += GLEW_STATIC
    LIBS += -lopengl32 -lglu32
}

SOURCES += ui/mainwindow.cpp \
    main.cpp \
    glew-1.10.0/src/glew.c \
    ui/view.cpp \
    ui/viewformat.cpp \
    camera/CamtransCamera.cpp \
    camera/OrbitingCamera.cpp \
    camera/QuaternionCamera.cpp \
    scenegraph/OpenGLScene.cpp \
    scenegraph/ShapesScene.cpp \
    scenegraph/Scene.cpp \
    shapes/cone.cpp \
    shapes/cube.cpp \
    shapes/cylinder.cpp \
    shapes/openglshape.cpp \
    shapes/shape.cpp \
    shapes/sphere.cpp \
    lib/BGRA.cpp \
    lib/CS123XmlSceneParser.cpp \
    lib/ResourceLoader.cpp \
    gl/datatype/FBO.cpp \
    gl/datatype/IBO.cpp \
    gl/datatype/VAO.cpp \
    gl/datatype/VBO.cpp \
    gl/datatype/VBOAttribMarker.cpp \
    gl/shaders/CS123Shader.cpp \
    gl/shaders/Shader.cpp \
    gl/textures/DepthBuffer.cpp \
    gl/textures/RenderBuffer.cpp \
    gl/textures/Texture.cpp \
    gl/textures/Texture2D.cpp \
    gl/textures/TextureParameters.cpp \
    gl/textures/TextureParametersBuilder.cpp \
    gl/util/FullScreenQuad.cpp \
    gl/GLDebug.cpp \
    scenegraph/SceneviewScene.cpp

HEADERS += ui/mainwindow.h \
    ui_mainwindow.h \
    glew-1.10.0/include/GL/glew.h \
    ui/view.h \
    ui/viewformat.h \
    camera/Camera.h \
    camera/CamtransCamera.h \
    camera/OrbitingCamera.h \
    camera/QuaternionCamera.h \
    scenegraph/OpenGLScene.h \
    scenegraph/ShapesScene.h \
    scenegraph/Scene.h \
    shapes/cone.h \
    shapes/cube.h \
    shapes/cylinder.h \
    shapes/openglshape.h \
    shapes/point.h \
    shapes/shape.h \
    shapes/sphere.h \
    lib/BGRA.h \
    lib/CS123ISceneParser.h \
    lib/CS123SceneData.h \
    lib/CS123XmlSceneParser.h \
    lib/ResourceLoader.h \
    gl/datatype/FBO.h \
    gl/datatype/IBO.h \
    gl/datatype/VAO.h \
    gl/datatype/VBO.h \
    gl/datatype/VBOAttribMarker.h \
    gl/shaders/CS123Shader.h \
    gl/shaders/Shader.h \
    gl/shaders/ShaderAttribLocations.h \
    gl/textures/DepthBuffer.h \
    gl/textures/RenderBuffer.h \
    gl/textures/Texture.h \
    gl/textures/Texture2D.h \
    gl/textures/TextureParameters.h \
    gl/textures/TextureParametersBuilder.h \
    gl/util/FullScreenQuad.h \
    gl/GLDebug.h \
    scenegraph/SceneviewScene.h

FORMS += ui/mainwindow.ui
INCLUDEPATH += glm ui glew-1.10.0/include
DEPENDPATH += glm ui glew-1.10.0/include

DEFINES += _USE_MATH_DEFINES
DEFINES += TIXML_USE_STL
DEFINES += GLM_SWIZZLE GLM_FORCE_RADIANS
OTHER_FILES += shaders/shader.frag \
    shaders/shader.vert \
    shaders/depth.frag \
    shaders/depth.vert

# Don't add the -pg flag unless you know what you are doing. It makes QThreadPool freeze on Mac OS X
QMAKE_CXXFLAGS_RELEASE -= -O2
QMAKE_CXXFLAGS_RELEASE += -O3
QMAKE_CXXFLAGS_WARN_ON -= -Wall
QMAKE_CXXFLAGS_WARN_ON += -Waddress -Warray-bounds -Wc++0x-compat -Wchar-subscripts -Wformat\
                          -Wmain -Wmissing-braces -Wparentheses -Wreorder -Wreturn-type \
                          -Wsequence-point -Wsign-compare -Wstrict-overflow=1 -Wswitch \
                          -Wtrigraphs -Wuninitialized -Wunused-label -Wunused-variable \
                          -Wvolatile-register-var -Wno-extra

QMAKE_CXXFLAGS += -g

# QMAKE_CXX_FLAGS_WARN_ON += -Wunknown-pragmas -Wunused-function -Wmain

macx {
    QMAKE_CXXFLAGS_WARN_ON -= -Warray-bounds -Wc++0x-compat
}

RESOURCES += \
    resources.qrc

DISTFILES += \
    shaders/normals/normals.vert \
    shaders/normals/normals.frag \
    shaders/normals/normals.gsh \
    shaders/normals/normalsArrow.gsh \
    shaders/normals/normalsArrow.frag \
    shaders/normals/normalsArrow.vert
