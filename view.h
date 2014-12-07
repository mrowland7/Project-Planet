#ifndef VIEW_H
#define VIEW_H

#include "shapes/ShapesScene.h"
#include <qgl.h>
#include <QTime>
#include <QTimer>

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

    void mousePressEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void wheelEvent(QWheelEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);

    ShapesScene* m_scene;
    Camera* m_camera;

private slots:
    void tick();
};

#endif // VIEW_H

