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
    m_currShape = new Sphere(100, 100);
    m_currShape->initOpenglData(m_shader);
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

//    ShapesScene::settingsChanged();

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

