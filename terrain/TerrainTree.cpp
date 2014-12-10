#include "TerrainTree.h"

TerrainTree::TerrainTree(GLint shader) {
    m_root = new Chunk(1, glm::vec2(0,0), 1);
    m_root->generate();



    m_shader = shader;
    Chunk::normalAttribLoc = glGetAttribLocation(m_shader, "normal");
    Chunk::positionAttribLoc = glGetAttribLocation(m_shader, "position");

    int x = glGetAttribLocation(m_shader, "normal");
    int y = glGetAttribLocation(m_shader, "position");

    int debughelper = 10;

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
