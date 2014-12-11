#include "TerrainTree.h"

TerrainTree::TerrainTree(GLint shader) {
    m_root = new Chunk(1, glm::vec2(0,0), 1, shader);
    m_root->generateRoot();
    m_model = glm::mat4();

}


TerrainTree::~TerrainTree() {
    delete m_root;
}

void TerrainTree::update(glm::vec3 cameraPos, float thetaWidth, float thetaHeight) {
    int level = getLevel(cameraPos, thetaWidth, thetaHeight);
    m_root->update(cameraPos, thetaWidth, thetaHeight, level);

}

void TerrainTree::draw(glm::vec3 cameraPos, float thetaWidth, float thetaHeight) {


    int level = getLevel(cameraPos, thetaWidth, thetaHeight);
    m_root->draw();
    //m_root.drawRecursive(cameraPos, thetaWidth, thetaHeight, level);



}

int TerrainTree::getLevel(glm::vec3 cameraPos, float thetaWidth, float thetaHeight){
    return 1;
}

glm::mat4 TerrainTree::getModel() {
    return m_model;
}

void TerrainTree::setModel(glm::mat4 model){
    m_model = model;
}
