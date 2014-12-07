#include "Scene.h"
#include "camera/Camera.h"
#include "CS123SceneData.h"
#include "CS123ISceneParser.h"
#include <QtGlobal>
#include <deque>


Scene::Scene() //: m_camera(NULL)
{
}

Scene::~Scene()
{
    // Do not delete m_camera, it is owned by SupportCanvas3D
//    for (ShapeInfo* s : m_shapeInfo) {
    for (int i = 0; i < m_shapeInfo.size(); i++) {
        ShapeInfo* s = m_shapeInfo.at(i);
        delete s;
    }

}

void Scene::parse(Scene *sceneToFill, CS123ISceneParser *parser)
{
    // TODO: load scene into sceneToFill using setGlobal(), addLight(), addPrimitive(), and
    //   finishParsing()
    CS123SceneGlobalData globalData;
    parser->getGlobalData(globalData);
    sceneToFill->setGlobal(globalData);

    CS123SceneLightData lightData;
    int numLights = parser->getNumLights();
    for (int i = 0; i < numLights; i++) {
        parser->getLightData(i, lightData);
        sceneToFill->addLight(lightData);
    }

    CS123SceneNode* root = parser->getRootNode();
    std::vector<NodeTransTuple*> toVisit;

    toVisit.push_back(new NodeTransTuple(root, glm::mat4x4()));

    int numToVisit = 1;
    while(numToVisit > 0) {
        NodeTransTuple* curr = toVisit.back(); //toVisit.pop_front();
        toVisit.pop_back();
        CS123SceneNode* node = curr->node;
        glm::mat4x4 trans = curr->trans;

//        // update current transformation
////        for (CS123SceneTransformation* t : node->transformations) {
//        for (int i = 0; i < node->transformations.size(); i++) {
//            CS123SceneTransformation* t = node->transformations.at(i);
//            glm::mat4x4 mat = getMat(t);
//            trans = trans * mat;
//        }

//        // Add the primitives here to the scene
////        for (CS123ScenePrimitive* p : node->primitives) {
//        for (int i = 0; i < node->primitives.size(); i++) {
//            CS123ScenePrimitive* p = node->primitives.at(i);
//            sceneToFill->addPrimitive(*p, trans);
//        }

//        // add the children
////        for(CS123SceneNode* n : node->children)  {
//        for (int i = 0; i < m_shapeInfo.size(); i++) {
//            ShapeInfo* s = m_shapeInfo.at(i);
//            toVisit.push_back(new NodeTransTuple(n, trans));
//            numToVisit++;
//        }

        numToVisit--;
        delete curr;
    }
}

// Parse into two scenes at once; much better for rayscene
void Scene::parse2(Scene *sceneToFill1, Scene *sceneToFill2, CS123ISceneParser *parser)
{
    // TODO: load scene into sceneToFill using setGlobal(), addLight(), addPrimitive(), and
    //   finishParsing()
    CS123SceneGlobalData globalData;
    parser->getGlobalData(globalData);
    sceneToFill1->setGlobal(globalData);
    sceneToFill2->setGlobal(globalData);

    CS123SceneLightData lightData;
    int numLights = parser->getNumLights();
    for (int i = 0; i < numLights; i++) {
        parser->getLightData(i, lightData);
        sceneToFill1->addLight(lightData);
        sceneToFill2->addLight(lightData);
    }

    CS123SceneNode* root = parser->getRootNode();
    std::vector<NodeTransTuple*> toVisit;

    toVisit.push_back(new NodeTransTuple(root, glm::mat4x4()));

    int numToVisit = 1;
    while(numToVisit > 0) {
        NodeTransTuple* curr = toVisit.back(); //toVisit.pop_front();
        toVisit.pop_back();
        CS123SceneNode* node = curr->node;
        glm::mat4x4 trans = curr->trans;

//        // update current transformation
//        for (CS123SceneTransformation* t : node->transformations) {
//            glm::mat4x4 mat = getMat(t);
//            trans = trans * mat;
//        }

//        // Add the primitives here to the scene
//        for (CS123ScenePrimitive* p : node->primitives) {
//            sceneToFill1->addPrimitive(*p, trans);
//            sceneToFill2->addPrimitive(*p, trans);
//        }

//        // add the children
//        for(CS123SceneNode* n : node->children)  {
//            toVisit.push_back(new NodeTransTuple(n, trans));
//            numToVisit++;
//        }

        numToVisit--;
        delete curr;
    }
}

void Scene::addPrimitive(const CS123ScenePrimitive &scenePrimitive, const glm::mat4x4 &matrix)
{

    CS123SceneFileMap* map = new CS123SceneFileMap();
    map->isUsed = scenePrimitive.material.textureMap->isUsed;
    map->repeatU = scenePrimitive.material.textureMap->repeatU;
    map->repeatV = scenePrimitive.material.textureMap->repeatV;
    map->filename = scenePrimitive.material.textureMap->filename;

    CS123SceneMaterial* material = new CS123SceneMaterial();
    material->textureMap = map;
    material->cDiffuse = scenePrimitive.material.cDiffuse;
    material->cAmbient = scenePrimitive.material.cAmbient;
    material->cReflective = scenePrimitive.material.cReflective;
    material->cSpecular = scenePrimitive.material.cSpecular;
    material->cEmissive = scenePrimitive.material.cEmissive;
    material->blend = scenePrimitive.material.blend;
    material->shininess = scenePrimitive.material.shininess;
    material->ior = scenePrimitive.material.ior;
    m_shapeInfo.push_back(new ShapeInfo(
                           scenePrimitive.type,
                           *material,
                           matrix));
}

void Scene::addLight(const CS123SceneLightData &sceneLight)
{
    m_lights.push_back(sceneLight);
}

void Scene::setGlobal(const CS123SceneGlobalData &global)
{
    m_global = global;
}


glm::mat4x4 Scene::getMat(CS123SceneTransformation* transf) {
    glm::mat4x4 mat;
    switch (transf->type) {
    case TRANSFORMATION_SCALE:
        mat = glm::transpose(glm::mat4x4(
                                 transf->scale.x, 0, 0, 0,
                                 0, transf->scale.y, 0, 0,
                                 0, 0, transf->scale.z, 0,
                                 0, 0, 0, 1
                                 ));
        break;
    case TRANSFORMATION_MATRIX:
        mat = transf->matrix;
        break;
    case TRANSFORMATION_TRANSLATE:
        mat = glm::transpose(
                    glm::mat4x4(
                                 1, 0, 0, transf->translate.x,
                                 0, 1, 0, transf->translate.y,
                                 0, 0, 1, transf->translate.z,
                                 0, 0, 0, 1
                                 )
                    );
        break;
    case TRANSFORMATION_ROTATE:
        mat = rotRod(transf->angle, transf->rotate);
        break;
    }

    return mat;
}


glm::mat4x4 Scene::rotRod(float angle, glm::vec3 axis) {
    double ux = axis.x;
    double uy = axis.y;
    double uz = axis.z;
    return glm::transpose(glm::mat4x4(
                             cos(angle)+ux*ux*(1-cos(angle)),    ux*uy*(1-cos(angle))-uz*sin(angle), ux*uz*(1-cos(angle))+uy*sin(angle), 0,
                             ux*uy*(1-cos(angle))+uz*sin(angle), cos(angle)+uy*uy*(1-cos(angle)),    uy*uz*(1-cos(angle))-ux*sin(angle), 0,
                             ux*uz*(1-cos(angle))-uy*sin(angle), uy*uz*(1-cos(angle))+ux*sin(angle), cos(angle)+uz*uz*(1-cos(angle)), 0,
                             0, 0, 0, 1
                             ));
}
