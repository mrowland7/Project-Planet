#include "view.h"
#include <QApplication>
#include <QKeyEvent>
#include "shapes/sphere.h"
#include "camera/CamtransCamera.h"
#include "ResourceLoader.h"

View::View(QWidget *parent) : QGLWidget(parent)
{
    // View needs all mouse move events, not just mouse drag events
    setMouseTracking(true);

    // Hide the cursor since this is a fullscreen app
    //setCursor(Qt::BlankCursor);

    // View needs keyboard focus
    setFocusPolicy(Qt::StrongFocus);

    // The game loop is implemented using a timer
    connect(&timer, SIGNAL(timeout()), this, SLOT(tick()));

    m_tree = 0;


    m_sunCamera = new CamtransCamera();
//    m_sunCamera->orientLook(glm::vec4(0, 0, 2, 0),
//                            glm::vec4(0, 0, -2, 0),
//                            glm::vec4(0, 1, 0, 0));
//    m_sunCamera->setClip(.00001,10);
    m_camera = new CamtransCamera();
//    m_camera->orientLook(glm::vec4(0, 0, 2, 0),
//                            glm::vec4(0, 0, -2, 0),
//                            glm::vec4(0, 1, 0, 0));
//    m_camera->setClip(.00001,10);
//    m_sunCamera->setHeightAngle(125);
    // mike test
//    m_camera->orientLook(glm::vec4(0, 1, 2, 0),
//                            glm::vec4(0, -1, -2, 0),
//                            glm::vec4(0, 1, 0, 0));
//    m_camera->orientLook(glm::vec4(2, 2, 4, 0),
//                            glm::vec4(-2, -2, -4, 0),
//                            glm::vec4(0, 1, 0, 0));
//    m_camera->orientLook(glm::vec4(0, 0, 2, 0),
//                            glm::vec4(0, 0, -1, 0),
//                            glm::vec4(0, 1, 0, 0));
    m_sunCamera->orientLook(glm::vec4(0, 1, 2, 0),
                                glm::vec4(0, -1, -2, 0),
                                glm::vec4(0, 1, 0, 0));
    m_camera->orientLook(glm::vec4(0, 1, 2, 0),
                                glm::vec4(0, -1, -2, 0),
                                glm::vec4(0, 1, 0, 0));
    m_camera->orientLook(glm::vec4(2, 2, 4, 0),
                                glm::vec4(-2, -2, -4, 0),
                                glm::vec4(0, 1, 0, 0));
}

View::~View()
{
    //delete m_tree;
}

void View::initializeGL()
{
    // All OpenGL initialization *MUST* be done during or after this
    // method. Before this method is called, there is no active OpenGL
    // context and all OpenGL calls have no effect.

    // Start a timer that will try to get 60 frames per second (the actual
    // frame rate depends on the operating system and other running programs)
    time.start();
    timer.start(1000 / 60);

    // Center the mouse, which is explained more in mouseMoveEvent() below.
    // This needs to be done here because the mouse may be initially outside
    // the fullscreen window and will not automatically receive mouse move
    // events. This occurs if there are two monitors and the mouse is on the
    // secondary monitor.
    //QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));


    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    glGetError(); // Clear errors after call to glewInit
    if (GLEW_OK != err)
    {
      // Problem: glewInit failed, something is seriously wrong.
      fprintf(stderr, "Error initializing glew: %s\n", glewGetErrorString(err));
    }

    initShaderInfo();

    // Enable depth testing, so that objects are occluded based on depth instead of drawing order.
    glEnable(GL_DEPTH_TEST);
    // Move the polygons back a bit so lines are still drawn even though they are coplanar with the
    // polygons they came from, which will be drawn before them.
    glEnable(GL_POLYGON_OFFSET_LINE);
    glPolygonOffset(-1, -1);
    // Enable back-face culling, meaning only the front side of every face is rendered.
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    // Specify that the front face is represented by vertices in counterclockwise order (this is
    // the default).
    glFrontFace(GL_CCW);


    initSquare();
    // TODO: init chunks here
    m_tree = new TerrainTree(m_shader, m_shadowmapShader);


    initShadowmapBuffers();



}

void View::initShaderInfo() {
    m_shader = ResourceLoader::loadShaders(
            ":/shaders/shader.vert",
            ":/shaders/shader.frag");

    m_shadowmapShader = ResourceLoader::loadShaders(
            ":/shaders/shadowmap.vert",
            ":/shaders/shadowmap.frag");

    m_uniformLocs["p"]= glGetUniformLocation(m_shader, "p");
    m_uniformLocs["m"]= glGetUniformLocation(m_shader, "m");
    m_uniformLocs["v"]= glGetUniformLocation(m_shader, "v");
    m_uniformLocs["mvp"]= glGetUniformLocation(m_shader, "mvp"); // TODO don't need this?
    m_uniformLocs["useLighting"]= glGetUniformLocation(m_shader, "useLighting");
    m_uniformLocs["tex"] = glGetUniformLocation(m_shader, "tex");
    m_uniformLocs["lightPosition"] = glGetUniformLocation(m_shader, "lightPosition");
    m_uniformLocs["lightColor"] = glGetUniformLocation(m_shader, "lightColor");


}

void View::initShadowmapBuffers() {
    glGenFramebuffers( 1, &m_shadowmapFBO );
    glBindFramebuffer( GL_FRAMEBUFFER, m_shadowmapFBO);

    glActiveTexture( GL_TEXTURE0 );
    glGenTextures( 1, &m_shadowmapColorAttachment);
    glBindTexture( GL_TEXTURE_2D, m_shadowmapColorAttachment );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width(), height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_shadowmapColorAttachment, 0);

    glGenRenderbuffers( 1, &m_shadowmapDepthAttachment );
    glBindRenderbuffer( GL_RENDERBUFFER, m_shadowmapDepthAttachment );
    glRenderbufferStorage( GL_RENDERBUFFER, GL_DEPTH_COMPONENT32, width(), height() );
    glFramebufferRenderbuffer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_shadowmapDepthAttachment );
//    glActiveTexture( GL_TEXTURE0 );
//    glGenTextures( 1, &m_shadowmapDepthAttachment );
//    glBindTexture( GL_TEXTURE_2D, m_shadowmapDepthAttachment);
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
//    glTexImage2D( GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0 );
//    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, m_shadowmapDepthAttachment, 0);
//    // No color buffer
//    glDrawBuffer(0);

    glBindFramebuffer( GL_FRAMEBUFFER, 0);
}

void View::paintGL()
{
    renderShadowmap();
    renderFinal();
}


void View::renderShadowmap() {
    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowmapFBO);
    renderFromCamera(m_sunCamera, m_shadowmapShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void View::renderFinal() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glActiveTexture(GL_TEXTURE0);
    glUniform1i(glGetUniformLocation(m_shader, "tex"), 0);
    glBindTexture(GL_TEXTURE_2D, m_shadowmapColorAttachment);
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );

    renderFromCamera(m_camera, m_shader);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void View::renderFromCamera(CamtransCamera* camera, GLuint shader) {
    glUseProgram(shader);
    glClearColor(0.05, 0.1, 0.2, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // TODO: bad bad bad bad bad
    glm::mat4x4 shadowV = m_sunCamera->getProjectionMatrix() * m_sunCamera->getViewMatrix();
    glUniformMatrix4fv(glGetUniformLocation(shader, "shadow_v"), 1, GL_FALSE, &shadowV[0][0]);


    glUniformMatrix4fv(glGetUniformLocation(shader, "p"), 1, GL_FALSE,
            glm::value_ptr(camera->getProjectionMatrix()));
    glUniformMatrix4fv(glGetUniformLocation(shader, "v"), 1, GL_FALSE,
            glm::value_ptr(camera->getViewMatrix()));
    glUniformMatrix4fv(m_uniformLocs["m"], 1, GL_FALSE,
            glm::value_ptr(m_tree->getModel()));

//    // TODO: instead of rendering square, do chunk rendering here
    glBindVertexArray(m_vaoID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawArrays(GL_TRIANGLES, 6, 6);
    glDrawArrays(GL_TRIANGLES, 12, 6);
    glBindVertexArray(0);

    //setLights
//    LightData ld = {0, glm::vec3(1,1,0),glm::vec3(m_camera->getPosition())};
//    this->setLight(ld);

//    m_tree->draw(glm::vec3(), 0, 0, shader);
}

void View::initSquare() {
    GLuint vertexLocation1 = glGetAttribLocation(m_shader, "position");
    GLuint vertexLocation2 = glGetAttribLocation(m_shadowmapShader, "position");

    GLfloat vertexBufferData[] = {
//        // Square 1
        0.5f, -0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
       -0.5f, 0.5f, 0.0f,
       -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
       -0.5f, 0.5f, 0.0f,
        // Square 3
        1.8f, 0.0f, -4.0f,
        1.8f, 1.8f, -4.0f,
        0.8f, 1.8f, -4.0f,
        0.8f, 0.0f, -4.0f,
        1.8f, 0.0f, -4.0f,
        0.8f, 1.8f, -4.0f,
        // Square 2
        0.8f, -0.2f, -2.0f,
        0.8f, 0.8f, -2.0f,
       -0.2f, 0.8f, -2.0f,
       -0.2f, -0.2f, -2.0f,
        0.8f, -0.2f, -2.0f,
       -0.2f, 0.8f, -2.0f,
    };

    glGenVertexArrays(1, &m_vaoID);
    glBindVertexArray(m_vaoID);

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(vertexLocation1);
    glVertexAttribPointer(vertexLocation1,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          (void*) 0);
    glEnableVertexAttribArray(vertexLocation2);
    glVertexAttribPointer(vertexLocation2,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          (void*) 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void View::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void View::mousePressEvent(QMouseEvent *event)
{
    m_prevMouseCoordinates = glm::vec2((float)event->x(), (float)event->y());
    m_leftMouseDown = true;
}

void View::mouseMoveEvent(QMouseEvent *event)
{
    if(m_leftMouseDown) {
        glm::vec2 currMouseCoordinates = glm::vec2((float)event->x(), (float)event->y());
        glm::vec4 hit1;
        glm::vec4 hit2;
        glm::vec4 currRay = glm::vec4(getRayFromScreenCoord(currMouseCoordinates),0);
        glm::vec4 prevRay = glm::vec4(getRayFromScreenCoord(m_prevMouseCoordinates),0);

        if(intersectSphere(m_tree->getModel(), m_camera->getPosition(), currRay, hit2) &&
        intersectSphere(m_tree->getModel(), m_camera->getPosition(), prevRay, hit1)) {
            glm::vec4 center = m_tree->getModel()*glm::vec4(0,0,0,1);
            glm::vec3 a = glm::vec3(hit1 - center);
            glm::vec3 b = glm::vec3(hit2 - center);
            glm::vec3 axis = glm::normalize(glm::cross(b,a));
            float angle = acos(glm::dot(a, b)/(glm::length(a)*glm::length(b)));


            glm::mat4 mat1 = glm::translate(glm::mat4(), glm::vec3(-center));
            glm::mat4 mat2 = glm::rotate(glm::mat4(), angle, glm::vec3(axis));
            glm::mat4 mat3 = glm::translate(glm::mat4(), glm::vec3(center));
            m_tree->setModel(mat3*mat2*mat1*m_tree->getModel());


        }


    m_prevMouseCoordinates = currMouseCoordinates;
    }



}

void View::mouseReleaseEvent(QMouseEvent *event)
{
    m_leftMouseDown = false;
}

void View::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) QApplication::quit();

    // TODO: Handle keyboard presses here
    if(event->key() == Qt::Key_W) {
        m_forward = true;
    }
    if(event->key() == Qt::Key_S) {
        m_backward = true;
    }

}

void View::keyReleaseEvent(QKeyEvent *event)
{
    if(event->key() == Qt::Key_W) {
        m_forward = false;
    }
    if(event->key() == Qt::Key_S) {
        m_backward = false;
    }
}

void View::wheelEvent(QWheelEvent *event) {


}

void View::tick()
{
    // Get the number of seconds since the last tick (variable update rate)
    float seconds = time.restart() * 0.001f;

    if(m_forward) {
        m_camera->translate(m_camera->getLook()*m_moveSpeed*seconds);
        m_sunCamera->translate(m_sunCamera->getLook()*m_moveSpeed*seconds);
    }
    if(m_backward) {
        m_camera->translate(-m_camera->getLook()*m_moveSpeed*seconds);
        m_sunCamera->translate(-m_sunCamera->getLook()*m_moveSpeed*seconds);
    }

    // TODO: Implement the demo update here

    // TODO: check if there's a new section visible, if so, generate more terrain
//    float newY = 1 + glm::sin(time.currentTime().second() * 1.0);
//    float newZ = 2 + glm::sin(time.currentTime().second() * 1.0);
//    m_sunCamera->orientLook(glm::vec4(0, newY, newZ, 0),
//                            glm::vec4(0, 0 - newY, 0 - newZ, 0),
//                            glm::vec4(0, 1, 0, 0));

    // Flag this view for repainting (Qt will call paintGL() soon after)
    update();
}

bool View::intersectSphere(const glm::mat4 &matrix, const glm::vec4 &origin, const glm::vec4 &ray, glm::vec4 &intersection)
{
    // Sphere intersection formula.
    glm::vec4 center = matrix * glm::vec4(0, 0, 0, 1);
    float radius = m_trackballRadius;
    glm::vec4 fromCenter = origin - center;
    float a = glm::length2(ray);
    float b = 2.0 * glm::dot(ray, fromCenter);
    float c = glm::length2(fromCenter) - radius*radius;
    float discriminant = b * b - 4.0 * a * c;
    if (discriminant >= 0)
    {
        float t = (-b - sqrtf(discriminant)) / (2 * a);
        intersection = origin + ray * t;
        return true;
    }
    return false;
}

glm::vec3 View::getRayFromScreenCoord(glm::vec2 mouse) {

    int x = (int)mouse.x;
    int y = (int) mouse.y;

    glm::vec4 pFilm = glm::vec4((2.f*x)/width() - 1, 1-(2.f*y)/height(), -1,  1);
    glm::mat4x4 viewingTransform = glm::inverse(m_camera->getViewMatrix());

    glm::vec3 pWorld = glm::vec3(viewingTransform*pFilm);

    glm::vec4 debug = m_camera->getPosition();
    glm::vec3 rayStart = glm::vec3(viewingTransform*glm::vec4(0, 0, 0, 1));
    return glm::normalize(pWorld - rayStart);


}

void View::setLight(const LightData &light)
{
    glUniform3f(m_uniformLocs["lightPosition"], light.pos.x, light.pos.y, light.pos.z);

    glUniform3f(m_uniformLocs["lightColor"], light.color.x, light.color.y, light.color.z);

}
