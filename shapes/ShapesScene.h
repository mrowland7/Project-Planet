#ifndef SHAPESSCENE_H
#define SHAPESSCENE_H

#include "OpenGLScene.h"
#include "shape.h"

/**
 *
 * @class ShapesScene
 *
 * A scene that is used to render a single shape.
 *
 * This scene has no notion of the scene graph, so it will not be useful to you in
 * assignments requiring the display of multiple shapes. Luckily, the first time you
 * will need that kind of functionality is in the Sceneview assignment... and we've
 * left that task up to you in the SceneviewScene class.
 *
 * By default, the ShapesScene displays only a single triangle. You'll need to do
 * a little work here to render your shapes. You could render the shapes directly
 * in this class, or you could pass the work on to one or more subclasses. Think
 * carefully about your design here - you'll be reusing your shapes multiple times
 * during this course!
 */
class ShapesScene : public OpenGLScene
{
public:
    ShapesScene();
    virtual ~ShapesScene();

    // Initialize the scene.
    virtual void init();

protected:
    // Set the light uniforms for the lights in the scene. (The view matrix is used so that the
    // light can follows the camera.)
    virtual void setLights(const glm::mat4 viewMatrix);

    // Render geometry for Shapes and Sceneview.
    virtual void renderGeometry();

private:
    // Storage for private copies of the scene's light and material data. Note that these don't
    // need to be freed because they are VALUE types (not pointers) and the memory for them is
    // freed when the class itself is freed.
    CS123SceneLightData  m_light;
    CS123SceneMaterial   m_material;

    GLuint m_vaoID; // The vertex array object ID, which is passed to glBindVertexArray.
    Shape* m_currShape;

};

#endif // SHAPESSCENE_H
