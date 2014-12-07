#ifndef SCENE_H
#define SCENE_H

#include <CS123Common.h>
#include "CS123SceneData.h"
#include "shape.h"

class Camera;
class CS123ISceneParser;


struct NodeTransTuple
{
    CS123SceneNode* node;
    glm::mat4x4 trans;

    NodeTransTuple(CS123SceneNode* n, glm::mat4x4 m) :
        node(n), trans(m) {}
};

struct ShapeInfo {
    PrimitiveType prim;
//    CS123SceneColor diffuse;
//    CS123SceneColor ambient;
//    CS123SceneFileMap* tex;
    CS123SceneMaterial mat;
    glm::mat4x4 trix;

    ShapeInfo(PrimitiveType p, CS123SceneMaterial material, glm::mat4x4 matrix) :
        prim(p), mat(material), trix(matrix) {}

//    ~ShapeInfo() {
//        delete tex;
//    }
};


/**
 * @class Scene
 *
 * @brief This is the base class for all scenes. Modify this class if you want to provide
 * common functionality to all your scenes.
 */
class Scene
{
public:
    Scene();
    virtual ~Scene();

    static void parse(Scene *sceneToFill, CS123ISceneParser *parser);
    static void parse2(Scene *sceneToFill1, Scene *sceneToFill2, CS123ISceneParser *parser);

protected:

    // Adds a primitive to the scene.
    virtual void addPrimitive(const CS123ScenePrimitive &scenePrimitive, const glm::mat4x4 &matrix);

    // Adds a light to the scene.
    virtual void addLight(const CS123SceneLightData &sceneLight);

    // Sets the global data for the scene.
    virtual void setGlobal(const CS123SceneGlobalData &global);

    static glm::mat4x4 getMat(CS123SceneTransformation* transf);
    static glm::mat4x4 rotRod(float angle, glm::vec3 axis);

    CS123SceneGlobalData m_global;
    std::vector<ShapeInfo*> m_shapeInfo;
    std::vector<CS123SceneLightData> m_lights;

};

#endif // SCENE_H
