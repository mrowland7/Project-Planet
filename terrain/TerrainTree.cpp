#include "TerrainTree.h"

TerrainTree::TerrainTree(GLint shader, GLint shadowShader) {
    m_root = new Chunk(1, glm::vec2(0,0), 1, shader, shadowShader);
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

void TerrainTree::draw(glm::vec3 cameraPos, float thetaWidth, float thetaHeight, GLint shader) {
    int level = getLevel(cameraPos, thetaWidth, thetaHeight);
    m_root->drawRecursive(cameraPos, thetaWidth, thetaHeight, level, shader);

}

int TerrainTree::getLevel(glm::vec3 cameraPos, float thetaWidth, float thetaHeight){
    float dist = glm::length(cameraPos)-getHeight();
    int level = (int)-log(dist/5.f)/log(2);
    return 1;
    return glm::max(1, level);
}

glm::mat4 TerrainTree::getModel() {
    return m_model;
}

void TerrainTree::setModel(glm::mat4 model){
    m_model = model;
}

float TerrainTree::getHeight() {
    return m_root->MAX_MOUNTAIN_HEIGHT + .5;
}

glm::vec3 TerrainTree::getLoc() {
    return glm::vec3(m_model * glm::vec4(0,0,0,1));
}
