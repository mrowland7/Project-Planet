#ifndef VIEW_H
#define VIEW_H

#include "shapes/ShapesScene.h"
#include <qgl.h>
#include <QTime>
#include <QTimer>
#include "camera/CamtransCamera.h"
#include "terrain/TerrainTree.h"

class View : public QGLWidget
{
    Q_OBJECT

public:
    View(QWidget *parent);
    ~View();

private:
    QTime time;
    QTimer timer;

    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    void initShaderInfo();

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    void initSquare();

    ShapesScene* m_scene;
    CamtransCamera* m_camera;
    GLuint m_shader;
    // TODO: shouldn't have own vao, just for the sanity square
    GLuint m_vaoID;
    std::map<string, GLint> m_uniformLocs;

    TerrainTree *m_tree;

private slots:
    void tick();
};

#endif // VIEW_H

