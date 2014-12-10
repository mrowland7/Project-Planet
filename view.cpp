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
    setCursor(Qt::BlankCursor);

    // View needs keyboard focus
    setFocusPolicy(Qt::StrongFocus);

    // The game loop is implemented using a timer
    connect(&timer, SIGNAL(timeout()), this, SLOT(tick()));

    m_sunCamera = new CamtransCamera();
    m_sunCamera->translate(glm::vec4(3, 1, 3, 0));
    m_camera = new CamtransCamera();
}

View::~View()
{
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
    QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));


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

    // TODO: init chunks here instead of square
    initSquare();

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

}

void View::initShadowmapBuffers() {
    glGenFramebuffers( 1, &m_shadowmapFBO );
    glBindFramebuffer( GL_FRAMEBUFFER, m_shadowmapFBO);

    glActiveTexture( GL_TEXTURE0 );
    glGenTextures( 1, &m_shadowmapColorAttachment );
    glBindTexture( GL_TEXTURE_2D, m_shadowmapColorAttachment);
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, width(), height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL );
    glFramebufferTexture2D( GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_shadowmapColorAttachment, 0);

    glBindFramebuffer( GL_FRAMEBUFFER, 0);
}

void View::paintGL()
{
    renderShadowmap();
  //  renderFinal();
}


void View::renderShadowmap() {
//    glBindFramebuffer(GL_FRAMEBUFFER, m_shadowmapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    renderFromCamera(m_sunCamera, m_shadowmapShader);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void View::renderFinal() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    renderFromCamera(m_camera, m_shader);
}

void View::renderFromCamera(CamtransCamera* camera, GLuint shader) {
    glClearColor(0.05, 0.1, 0.2, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shader);
    glm::mat4 m4 = glm::mat4(1.0f);
    glUniform3f(glGetUniformLocation(shader, "color"), 0.4, 0.5, 0);
    glUniformMatrix4fv(glGetUniformLocation(shader, "mvp"), 1, GL_FALSE, &m4[0][0]);

    glm::mat4 viewMatrix = camera->getViewMatrix();
    glUniform1i(m_uniformLocs["useLighting"], true);
    glUniformMatrix4fv(m_uniformLocs["p"], 1, GL_FALSE,
            glm::value_ptr(camera->getProjectionMatrix()));
    glUniformMatrix4fv(m_uniformLocs["v"], 1, GL_FALSE,
            glm::value_ptr(viewMatrix));
    glUniformMatrix4fv(m_uniformLocs["m"], 1, GL_FALSE,
            glm::value_ptr(glm::mat4()));

    // TODO: instead of rendering square, do chunk rendering here
    glBindVertexArray(m_vaoID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

}

void View::initSquare() {
    // Step 1: Create GLfloat array of vertices that will draw a square. This should involve 6 vertices
    // Here is an example that contains the vertices of an isosceles triangle
    GLuint vertexLocation = glGetAttribLocation(m_shader, "position");

    GLfloat vertexBufferData[] = {
        0.5f, -0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
       -0.5f, 0.5f, 0.0f,
       -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
       -0.5f, 0.5f, 0.0f,
    };

    // Step 2: initialize and bind a Vertex Array Object -- see glGenVertexArrays and glBindVertexArray
    glGenVertexArrays(1, &m_vaoID);
    glBindVertexArray(m_vaoID);


    // Step 3: initialize and bind a buffer for your vertex data -- see glGenBuffers and glBindBuffer
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    // Step 4: Send your vertex data to the GPU -- see glBufferData
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);

    // Step 5: Expose the vertices to other OpenGL components (namely, shaders)
    //         -- see glEnableVertexAttribArray and glVertexAttribPointer
    glEnableVertexAttribArray(vertexLocation);
    glVertexAttribPointer(vertexLocation,
                          3,
                          GL_FLOAT,
                          GL_FALSE,
                          0,
                          (void*) 0);

    // Step 6: Clean up -- unbind the buffer and vertex array.
    //         It is a good habit to leave the state of OpenGL the way you found it
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void View::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void View::mousePressEvent(QMouseEvent *event)
{
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
    int deltaX = event->x() - width() / 2;
    int deltaY = event->y() - height() / 2;
    if (!deltaX && !deltaY) return;
    QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));

    // TODO: Handle mouse movements here
}

void View::mouseReleaseEvent(QMouseEvent *event)
{
}

void View::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape) QApplication::quit();

    // TODO: Handle keyboard presses here
}

void View::keyReleaseEvent(QKeyEvent *event)
{
}

void View::wheelEvent(QWheelEvent *event) {


}

void View::tick()
{
    // Get the number of seconds since the last tick (variable update rate)
    float seconds = time.restart() * 0.001f;

    // TODO: Implement the demo update here

    // TODO: check if there's a new section visible, if so, generate more terrain


    // Flag this view for repainting (Qt will call paintGL() soon after)
    update();
}
