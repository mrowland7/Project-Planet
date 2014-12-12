#ifndef VIEW_H
#define VIEW_H

#include "shapes/ShapesScene.h"
#include <qgl.h>
#include <QTime>
#include <QTimer>
#include "camera/CamtransCamera.h"
#include "terrain/TerrainTree.h"
#include <string>       // std::string
#include <iostream>     // std::cout
#include <sstream>      // std::ostringstream

class View : public QGLWidget
{
    Q_OBJECT

public:
    View(QWidget *parent);
    ~View();

    struct LightData{
        int index;
        glm::vec3 color;
        glm::vec3 pos;       // Not applicable to directional lights

    };

private:
    QTime time;
    QTimer timer;


    //VIEWING
    const float m_moveSpeed = 1.f;
    bool m_forward = false;
    bool m_backward = false;
    glm::vec2 m_prevMouseCoordinates;
    bool m_leftMouseDown = false;
    bool m_rightMouseDown = false;
    glm::vec3 getRayFromScreenCoord(glm::vec2 mouse);
    bool intersectSphere(const glm::mat4 &matrix, const glm::vec4 &origin, const glm::vec4 &ray, glm::vec4 &intersection);
    float m_trackballRadius = .5f;


    //GL
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    void renderFromCamera(CamtransCamera* camera, GLuint shader);
    void renderFinal();
    void renderShadowmap();
    void initShaderInfo();
    void initShadowmapBuffers();

    //EVENTS
    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void initSquare();

    ShapesScene* m_scene;
    CamtransCamera* m_camera;
    CamtransCamera* m_sunCamera;
    GLuint m_shader;
    GLuint m_shadowmapShader;
    GLuint m_shadowmapFBO;
    GLuint m_shadowmapColorAttachment;
    // TODO: shouldn't have own vao, just for the sanity square
    GLuint m_vaoID;
    std::map<string, GLint> m_uniformLocs;
    void setLight(const LightData &light);


    TerrainTree *m_tree;

private slots:
    void tick();
};

#endif // VIEW_H

