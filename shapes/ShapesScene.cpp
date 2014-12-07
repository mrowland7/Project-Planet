#include "ShapesScene.h"
#include "Camera.h"
#include <SupportCanvas3D.h>
#include <QFileDialog>
#include "shape.h"
#include "cube.h"
#include "cylinder.h"
#include "sphere.h"
#include "cone.h"
#include "torus.h"
#include "mobius.h"


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
    m_currShape = NULL;
    m_prevShape = -1;
    m_prevP1 = -1;
    m_prevP2 = -1;
    m_prevP3 = -1;

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

    ShapesScene::settingsChanged();

}

void ShapesScene::settingsChanged()
{
    int p1 = settings.shapeParameter1;
    int p2 = settings.shapeParameter2;
    int p3 = settings.shapeParameter3;

    if (p1 != m_prevP1 || p2 != m_prevP2 || settings.shapeType != m_prevShape
            || (settings.shapeType == SHAPE_TORUS && p3 != m_prevP3)) {
        delete m_currShape;
        switch (settings.shapeType)
        {
        case SHAPE_CUBE:
            m_currShape = new Cube(p1, p2);
            break;
        case SHAPE_CONE:
            m_currShape = new Cone(p1, p2);
            break;
        case SHAPE_SPHERE:
            m_currShape = new Sphere(p1, p2);
            break;
        case SHAPE_CYLINDER:
            m_currShape = new Cylinder(p1, p2);
            break;
        case SHAPE_TORUS:
            m_currShape = new Torus(p1, p2, p3);
            break;
        case SHAPE_SPECIAL_1:
            m_currShape = new Mobius(p1, p2);
            break;
        default:
            m_currShape = new Cube(p1, p2);
            break;
        }

        /*
        // Initialize the vertex array object.
        glGenVertexArrays(1, &m_vaoID);
        glBindVertexArray(m_vaoID);

        // Initialize the vertex buffer object.
        GLuint vertexBuffer;
        glGenBuffers(1, &vertexBuffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);


       // Unbind buffers.
       glBindBuffer(GL_ARRAY_BUFFER, 0);
       glBindVertexArray(0);
       glDeleteBuffers(1, &vertexBuffer);
       */
        m_currShape->initOpenglData(m_shader);

       m_currShape->renderNormals(m_normalRenderer);

       m_prevP1 = p1;
       m_prevP2 = p2;
       m_prevP3 = p3;
       m_prevShape = settings.shapeType;
    }

}

void ShapesScene::renderGeometry()
{
    // TODO: [SHAPES] Render the shape. The sample code provided draws the 3 vertices defined in
    //       init() above.

    applyMaterial(m_material);
    m_currShape->render(m_shader);

}

void ShapesScene::setLights(const glm::mat4 viewMatrix)
{
    // YOU DON'T NEED TO TOUCH THIS METHOD, unless you want to do fancy lighting...

    m_light.dir = glm::inverse(viewMatrix) * lightDirection;

    clearLights();
    setLight(m_light);
}

