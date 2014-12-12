#include "TerrainTree.h"

TerrainTree::TerrainTree(GLint shader, GLint shadowShader) {
    m_root = new Chunk(1, glm::vec2(0,0), 1, shader, shadowShader);
    m_root->generateRoot();
    m_model = glm::mat4();

}


TerrainTree::~TerrainTree() {
    delete m_root;
}

void TerrainTree::update(glm::vec3 cameraPos) {
    glm::vec3 relativeCameraPos = glm::vec3(glm::inverse(m_model)*glm::vec4(cameraPos, 1));
    int level = getLevel(relativeCameraPos);
    m_root->update(relativeCameraPos, level);

}

void TerrainTree::draw(glm::vec3 cameraPos, GLint shader) {
    glm::vec3 relativeCameraPos = glm::vec3(glm::inverse(m_model)*glm::vec4(cameraPos, 1));
    int level = getLevel(relativeCameraPos);
    level = 20; //TODO
    m_root->drawRecursive(relativeCameraPos, level, shader);

}

int TerrainTree::getLevel(glm::vec3 cameraPos){
    float dist = glm::length(cameraPos-getLoc())-getHeight();
    //int level = (int)(-log(dist/5.f)/log(2)); with height taking MAX_MOUNTAIN_HEIGHT into acount
    int level = (int)(-log(dist*m_root->VERTEX_GRID_WIDTH/256.f)/log(2));
    std::cout << level << std::endl;
    return glm::clamp(1, level,20);
}

glm::mat4 TerrainTree::getModel() {
    return m_model;
}

void TerrainTree::setModel(glm::mat4 model){
    m_model = model;
}

float TerrainTree::getHeight() {
    return /*m_root->MAX_MOUNTAIN_HEIGHT + */m_root->RADIUS*1.001;
}

glm::vec3 TerrainTree::getLoc() {
    return glm::vec3(m_model * glm::vec4(0,0,0,1));
}
