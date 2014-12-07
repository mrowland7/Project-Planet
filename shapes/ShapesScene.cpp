#include "ShapesScene.h"
#include <QFileDialog>
#include "shape.h"
#include "sphere.h"


glm::vec4 lightDirection = glm::normalize(glm::vec4(1.f, -1.f, -1.f, 0.f));

ShapesScene::ShapesScene()
{
    // Use a shiny orange material
    memset(&m_material, 0, sizeof(m_material));
    m_material.cAmbient.r = 0.2f;
    m_material.cAmbient.g = 0.1f;
    m_material.cDiffuse.r = 1;
    m_material.cDiffuse.g = 0.5f;
    m_material.cSpecular.r = m_material.cSpecular.g = m_material.cSpecular.b = 1;
    m_material.shininess = 64;

    // Use a white directional light from the upper left corner
    memset(&m_light, 0, sizeof(m_light));
    m_light.type = LIGHT_DIRECTIONAL;
    m_light.dir = lightDirection;
    m_light.color.r = m_light.color.g = m_light.color.b = 1;
    m_light.id = 0;

    //TODO: [SHAPES] Allocate any additional memory you need...
    init();
    m_currShape = NULL;
//    m_currShape = new Sphere(100, 100);
//    m_currShape->initOpenglData(m_shader);
}

ShapesScene::~ShapesScene()
{
    // TODO: [SHAPES] Don't leak memory!
    delete m_currShape;
    // TODO: fix this thing
    //glDeleteVertexArrays(1, &m_vaoID);

}

void ShapesScene::init()
{
    // TODO: [SHAPES] Initialize the shape based on settings.shapeType. The sample code provided
    //       initializes a single triangle using OpenGL. Refer to the labs for more information
    //       about these OpenGL functions.
    //
    //       If you've chosen to implement subclasses for your shapes, this would be a good time to
    //       call upon the subclass to initialize itself.
    //

    OpenGLScene::init(); // Call the superclass's init()

    // Initialize the vertex array object.
    glGenVertexArrays(1, &m_vaoID);
    glBindVertexArray(m_vaoID);

    // Initialize the vertex buffer object.
    GLuint vertexBuffer;
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);

    // Remember for large arrays you should use new
    GLfloat vertexData[] = {
        -1, -1, 0, // Position 1
        0,  0, 1, // Normal 1
        1, -1, 0, // Position 2
        0,  0, 1, // Normal 2
        0, 1, 0,  // Position 3
        0, 0, 1,   // Normal 3
        1, -1, 0, // Position 2
        0,  0, 1, // Normal 2
        -1, -1, 0, // Position 1
        0,  0, 1, // Normal 1
        0, 1, 0,  // Position 3
        0, 0, 1   // Normal 3
   };

   // Pass vertex data to OpenGL.
   glBufferData(GL_ARRAY_BUFFER, 6 * 6 * sizeof(GLfloat), vertexData, GL_STATIC_DRAW);
   glEnableVertexAttribArray(glGetAttribLocation(m_shader, "position"));
   glEnableVertexAttribArray(glGetAttribLocation(m_shader, "normal"));
   glVertexAttribPointer(
       glGetAttribLocation(m_shader, "position"),
       3,                   // Num coordinates per position
       GL_FLOAT,            // Type
       GL_FALSE,            // Normalized
       sizeof(GLfloat) * 6, // Stride
       (void*) 0            // Array buffer offset
   );
   glVertexAttribPointer(
       glGetAttribLocation(m_shader, "normal"),
       3,           // Num coordinates per normal
       GL_FLOAT,    // Type
       GL_TRUE,     // Normalized
       sizeof(GLfloat) * 6,           // Stride
       (void*) (sizeof(GLfloat) * 3)    // Array buffer offset
   );

   // Unbind buffers.
   glBindBuffer(GL_ARRAY_BUFFER, 0);
   glBindVertexArray(0);
//    ShapesScene::settingsChanged();

}

void ShapesScene::renderGeometry()
{
    // TODO: [SHAPES] Render the shape. The sample code provided draws the 3 vertices defined in
    //       init() above.

    applyMaterial(m_material);
    glBindVertexArray(m_vaoID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);
//    m_currShape->render(m_shader);

}

void ShapesScene::setLights(const glm::mat4 viewMatrix)
{
    // YOU DON'T NEED TO TOUCH THIS METHOD, unless you want to do fancy lighting...

    m_light.dir = glm::inverse(viewMatrix) * lightDirection;

    clearLights();
    setLight(m_light);
}

