#include "view.h"
#include <QApplication>
#include <QKeyEvent>
#include "shapes/sphere.h"
#include "camera/CamtransCamera.h"
#include "ResourceLoader.h"

#define ORBIT_X 0
#define ORBIT_Y 1.5
#define ORBIT_Z 1.5

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
    m_camera = new CamtransCamera();
//    m_camera->orientLook(glm::vec4(2, 2, 0, 1),
//                            glm::vec4(-2, -2, 0, 0),
//                            glm::vec4(0, 1, 0, 0));
//    m_camera->setClip(.00001,10);
//    m_camera->setAspectRatio((float)width()/height());
//    m_sunCamera->setHeightAngle(125);
    // mike test
//    m_sunCamera->setClip(.00001,10);
//    m_camera->setClip(.00001,10);
    m_sunCamera->orientLook(glm::vec4(ORBIT_X, ORBIT_Y, ORBIT_Z, 0),
                                glm::vec4(-ORBIT_Z, -ORBIT_Y, -ORBIT_Z, 0),
                                glm::vec4(0, 1, 0, 0));
    m_camera->orientLook(glm::vec4(2, 2, 5, 0),
                                glm::vec4(-2, -2, -5, 0),
                                glm::vec4(0, 1, 0, 0));
    m_shadowsOn = true;
    m_showShadowmap = false;
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
    sendTextures(m_shader);
}

void View::initShaderInfo() {
    m_shader = ResourceLoader::loadShaders(
            ":/shaders/shader.vert",
            ":/shaders/shader.frag");

    m_shadowmapShader = ResourceLoader::loadShaders(
            ":/shaders/shadowmap.vert",
            ":/shaders/shadowmap.frag");
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
    if (!m_showShadowmap) {
        renderFinal();
    }
}


void View::renderShadowmap() {
    if (!m_showShadowmap) {
        glBindFramebuffer(GL_FRAMEBUFFER, m_shadowmapFBO);
    }
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
    // TODO: also bad bad bad
    if (shader == m_shader) {
        glUniformMatrix4fv(glGetUniformLocation(m_shader, "m"), 1, GL_FALSE,
                glm::value_ptr(m_tree->getModel()));
    } else {
        glUniformMatrix4fv(glGetUniformLocation(m_shadowmapShader, "m"), 1, GL_FALSE,
                glm::value_ptr(glm::mat4()));
    }

    if (shader == m_shader) {
        sendTexturesRender();
        glUniform1i(glGetUniformLocation(m_shader, "shadowsOn"),
                    m_shadowsOn ? 2 : 1);

    }

//    // TODO: instead of rendering square, do chunk rendering here
//    glBindVertexArray(m_vaoID);
//    glDrawArrays(GL_TRIANGLES, 0, 6);
//    glDrawArrays(GL_TRIANGLES, 6, 6);
//    glDrawArrays(GL_TRIANGLES, 12, 6);
//    glBindVertexArray(0);

    //setLights
    LightData ld = {0, glm::vec3(1,1,0), glm::vec3(m_sunCamera->getPosition())};
    this->setLight(ld);
    glUniform3f(glGetUniformLocation(m_shader, "objColor"), 1, 1, 1);

    m_tree->update(glm::vec3(m_camera->getPosition()));
    m_tree->draw(glm::vec3(m_camera->getPosition()), shader);
}

void View::initSquare()
{
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
    m_camera->setAspectRatio((float)w/h);

    glViewport(0, 0, w, h);
    initShadowmapBuffers();
}

void View::mousePressEvent(QMouseEvent *event)
{
    m_prevMouseCoordinates = glm::vec2((float)event->x(), (float)event->y());
    if(event->buttons() == Qt::LeftButton)
        m_leftMouseDown = true;
    if(event->buttons() == Qt::RightButton)
        m_rightMouseDown = true;
}

void View::mouseMoveEvent(QMouseEvent *event)
{
    glm::vec2 currMouseCoordinates = glm::vec2((float)event->x(), (float)event->y());

    if(m_leftMouseDown) {
        glm::vec4 hit1;
        glm::vec4 hit2;
        glm::vec4 currRay = glm::vec4(getRayFromScreenCoord(currMouseCoordinates),0);
        glm::vec4 prevRay = glm::vec4(getRayFromScreenCoord(m_prevMouseCoordinates),0);

        if(intersectSphere(m_tree->getModel(), m_camera->getPosition(), currRay, hit2) &&
        intersectSphere(m_tree->getModel(), m_camera->getPosition(), prevRay, hit1)) {
            glm::vec4 center = glm::vec4(m_tree->getLoc(),1);
            glm::vec3 a = glm::vec3(hit1 - center);
            glm::vec3 b = glm::vec3(hit2 - center);
            glm::vec3 axis = glm::normalize(glm::cross(a,b));
            float angle = acos(glm::dot(a, b)/(glm::length(a)*glm::length(b))-.0001f); //avoid floating point errors...


            glm::mat4 mat1 = glm::translate(glm::mat4(), glm::vec3(-center));
            glm::mat4 mat2 = glm::rotate(glm::mat4(), angle, glm::vec3(axis));
            glm::mat4 mat3 = glm::translate(glm::mat4(), glm::vec3(center));
            glm::vec4 eyePos = mat2*m_camera->getPosition();
            //m_camera->orientLook(eyePos, -eyePos, m_camera->getUp()); //rotate camera instead of object
            m_tree->setModel(mat3*mat2*mat1*m_tree->getModel());

        }
    }
    if(m_rightMouseDown) {
        glm::vec2 diffP = (currMouseCoordinates - m_prevMouseCoordinates);
        glm::vec2 dimP = glm::vec2(width(), height());
        float heightAngle = m_camera->getHeightAngle();
        glm::vec2 dimA = glm::vec2(heightAngle, m_camera->getAspectRatio()*heightAngle);
        glm::vec2 diff = (diffP/dimP)*dimA;
        m_camera->rotateV(diff.x);
        m_camera->rotateU(diff.y);

        //set up vector when close to planet surface?
        //glm::vec4 up = glm::normalize(m_camera->getPosition() - glm::vec4(0,0,0,1));
        //m_camera->orientLook(m_camera->getPosition(), m_camera->getLook(), up);
    }

    m_prevMouseCoordinates = currMouseCoordinates;


}

void View::mouseReleaseEvent(QMouseEvent *event)
{
        m_leftMouseDown = false;
        m_rightMouseDown = false;
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
    if(event->key() == Qt::Key_X) {
        m_shadowsOn = !m_shadowsOn;
    }
    if(event->key() == Qt::Key_Z) {
        m_showShadowmap = !m_showShadowmap;
    }
}

void View::wheelEvent(QWheelEvent *event) {
    QPoint angleDelta = event->angleDelta();
    float scale = pow(.95, (float)angleDelta.y()/60.f);
    float distToCenter = glm::length(m_camera->getPosition()-glm::vec4(m_tree->getLoc(),1));
    float transDist = (1.f-scale) *(distToCenter -m_tree->getHeight());

    m_camera->translate(m_camera->getLook()*transDist);

}

void View::tick()
{
    // Get the number of seconds since the last tick (variable update rate)
    float seconds = time.restart() * 0.001f;

    if(m_forward) {
        m_camera->translate(m_camera->getLook()*m_moveSpeed*seconds);
    }
    if(m_backward) {
        m_camera->translate(-m_camera->getLook()*m_moveSpeed*seconds);
    }

    // TODO: Implement the demo update here
    // position at time 0: y = 1, z = 2
    // opposite position: y = -1, z = -2

    float timeMsec = time.currentTime().msec() + time.currentTime().second() * 1000;
    float piTime = timeMsec * (2 * 3.1415926 / 1200000.0); // loop every two minutes?
    float adjustTimeY = glm::cos(piTime);
    float adjustTimeZ = glm::sin(piTime);
    float newY = adjustTimeY * ORBIT_Y;
    float newZ = adjustTimeZ * ORBIT_Z;

    m_sunCamera->orientLook(glm::vec4(ORBIT_X, newY, newZ, 0),
                            glm::vec4(-ORBIT_X, 0 - newY, 0 - newZ, 0),
                            glm::vec4(0, 1, 0, 0));

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
    float xN = (2.f*x)/width() - 1;
    float yN = 1-(2.f*y)/height();

    glm::vec4 pFilm = glm::vec4(xN, yN, -1,  1);
    glm::mat4x4 viewingTransform = glm::inverse(m_camera->getScaleMatrix()*m_camera->getViewMatrix());

    glm::vec3 pWorld = glm::vec3(viewingTransform*pFilm);

    glm::vec3 rayStart = glm::vec3(viewingTransform*glm::vec4(0, 0, 0, 1));
    return glm::normalize(pWorld - rayStart);


}

void View::setLight(const LightData &light)
{
    glUniform3f(glGetUniformLocation(m_shader, "lightPosition"), light.pos.x, light.pos.y, light.pos.z);

    glUniform3f(glGetUniformLocation(m_shader, "lightColor"), light.color.x, light.color.y, light.color.z);

}

void View::sendTextures(GLint shader) {

    glActiveTexture(GL_TEXTURE1);
    std::string snowPath = "/course/cs123/data/image/terrain/snow.JPG";
    GLuint snowTex = loadTexture(QString::fromStdString(snowPath));
    GLint snowLoc = glGetUniformLocation(m_shader, "snowTexture");
    glUniform1i(snowLoc, 1);
    glBindTexture(GL_TEXTURE_2D, snowTex);
    m_snowTex = snowTex;

    glActiveTexture(GL_TEXTURE2);
    std::string rockPath = ":/shaders/venus2.jpg";
    GLuint rockTex = loadTexture(QString::fromStdString(rockPath));
    GLint rockLoc = glGetUniformLocation(m_shader, "rockTexture");
    glUniform1i(rockLoc, 2);
    glBindTexture(GL_TEXTURE_2D, rockTex);
    m_rockTex = rockTex;

    glActiveTexture(GL_TEXTURE3);
    std::string lavaPath = ":/shaders/lava.png";
    GLuint lavaTex = loadTexture(QString::fromStdString(lavaPath));
    GLint lavaLoc = glGetUniformLocation(m_shader, "lavaTexture");
    glUniform1i(lavaLoc, 3);
    glBindTexture(GL_TEXTURE_2D, lavaTex);
    m_lavaTex = lavaTex;

    glActiveTexture(GL_TEXTURE4);
    std::string dirtPath = "/course/cs123/data/image/terrain/dirt.JPG";
    GLuint dirtTex = loadTexture(QString::fromStdString(dirtPath));
    GLint dirtLoc = glGetUniformLocation(m_shader, "dirtTexture");
    glUniform1i(dirtLoc, 4);
    glBindTexture(GL_TEXTURE_2D, dirtTex);
    m_dirtTex = dirtTex;

    glActiveTexture(GL_TEXTURE0);
}

void View::sendTexturesRender()
{
    glActiveTexture(GL_TEXTURE1);
    GLint snowLoc = glGetUniformLocation(m_shader, "snowTexture");
    glUniform1i(snowLoc, 1);
    glBindTexture(GL_TEXTURE_2D, m_snowTex);

    glActiveTexture(GL_TEXTURE2);
    GLint rockLoc = glGetUniformLocation(m_shader, "rockTexture");
    glUniform1i(rockLoc, 2);
    glBindTexture(GL_TEXTURE_2D, m_rockTex);

    glActiveTexture(GL_TEXTURE3);
    GLint lavaLoc = glGetUniformLocation(m_shader, "lavaTexture");
    glUniform1i(lavaLoc, 3);
    glBindTexture(GL_TEXTURE_2D, m_lavaTex);

    glActiveTexture(GL_TEXTURE4);
    GLint dirtLoc = glGetUniformLocation(m_shader, "dirtTexture");
    glUniform1i(dirtLoc, 4);
    glBindTexture(GL_TEXTURE_2D, m_dirtTex);

    glActiveTexture(GL_TEXTURE0);

}

GLuint View::loadTexture(const QString &path)
{
    QImage texture;
    QFile file(path);
    if(!file.exists()) return -1;
    texture.load(file.fileName());
    texture = QGLWidget::convertToGLFormat(texture);

    // Put your code here
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(GL_TEXTURE_2D,
                 0, //level
                 GL_RGBA, // internal format
                 texture.width(),
                 texture.height(),
                 0, // border... always 0
                 GL_RGBA, // format
                 GL_UNSIGNED_BYTE, // type of data,
                 texture.bits() // pointer to image data
                 );


    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER,
                    GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    return id; // Return something meaningful
}
