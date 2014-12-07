#include "chunk.h"

Chunk::Chunk()
{
    children[0] = 0;
    children[1] = 0;
    children[2] = 0;
    children[3] = 0;
    heightData = 0;
    biomeData = 0;


}

Chunk::~Chunk() {
    delete children[0];
    delete children[1];
    delete children[2];
    delete children[3];
    delete[] heightData;
    delete[] biomeData;
}


void Chunk::draw() {

}

void Chunk::drawRecursive(glm::vec3 cameraPos, float thetaWidth, float thetaHeight, int level) {\

    if(this->level <= level) {
        bool allChildrenExist = children[0] != 0 && children[1] != 0
                && children[2] != 0 && children[3] != 0;
        if(allChildrenExist) {
            for(int i = 0; i < 4; i++) {
                children[i]->draw();
            }
        } else {
            draw();
        }
    }


}

void Chunk::update(glm::vec3 cameraPos, float thetaWidth, float thetaHeight, int level) {

}

void Chunk::generate(float *parentHeightData, float *parentBiomeData, int quadrant) {
    this->heightData = new float[(vertexWidth+1)*(vertexHeight+1)];
    this->normalData = new float[(vertexWidth+1)*(vertexHeight+1)];

    int quadX = quadrant%2;
    int quadY = quadrant/2;
    int xOffset = quadX*vertexWidth/2;
    int yOffset = quadY*vertexHeight/2;

    for(int row = 0; row < vertexHeight/2 + 1; row++) {
        for(int col = 0; col < vertexWidth/2 + 1; col++) {
            heightData[(row*2)*vertexWidth + (col*2)] = parentHeightData[(row+yOffset)*vertexWidth + col+xOffset];
        }
    }


}

void Terrain::subdivideSquare(glm::vec2 topleft, glm::vec2 botright)
{
    // TL--TM--TR    +---> x
    // |   |   |     |
    // ML--MM--MR    V
    // |   |   |     y
    // BL--BM--BR
    glm::vec2 r = glm::vec2((botright.x - topleft.x)/2.f, (botright.y - topleft.y)/2.f);



    // Corner coordinates (in the grid space [x,y])
    glm::vec2 TL = glm::vec2(topleft.x, topleft.y);
    glm::vec2 TR = glm::vec2(botright.x, topleft.y);
    glm::vec2 BL = glm::vec2(topleft.x, botright.y);
    glm::vec2 BR = glm::vec2(botright.x, botright.y);



    // Corner vertices on the terrain (in the grid space [x,y,z])
    glm::vec3 &vTL = m_terrain[getIndex(TL)];
    glm::vec3 &vTR = m_terrain[getIndex(TR)];
    glm::vec3 &vBL = m_terrain[getIndex(BL)];
    glm::vec3 &vBR = m_terrain[getIndex(BR)];



    glm::vec2 TM = (TL + TR)*.5f;
    glm::vec2 RM = (TR + BR)*.5f;
    glm::vec2 BM = (BL + BR)*.5f;
    glm::vec2 LM = (TL + BL)*.5f;
    glm::vec2 MM = (TL + TR + BL + BR)*.25f;

    m_terrain[getIndex(TM)] = (vTL + vTR)*.5f;
    m_terrain[getIndex(RM)] = (vTR + vBR)*.5f;
    m_terrain[getIndex(BM)] = (vBL + vBR)*.5f;
    m_terrain[getIndex(LM)] = (vTL + vBL)*.5f;
    m_terrain[getIndex(MM)] = ((vTL + vTR + vBL + vBR)*.25f) + glm::vec3(0.f, getPerturb(), 0);

}

/**
 * Converts a grid coordinate (row, column) to an index into a 1-dimensional array.
 * Can be used to index into m_terrain or m_normalMap.
 * Returns -1 if the grid coordinate entered is not valid.
 */
inline int Terrain::getIndex(const glm::vec2 &c)
{
    assert(c.x - (int)c.x < .001);
    return getIndex(c.x, c.y);
}

/**
 * Converts a grid coordinate (row, column) to an index into a 1-dimensional array.
 * Can be used to index into m_terrain or m_normalMap.
 * Returns -1 if the grid coordinate entered is not valid.
 */
inline int Terrain::getIndex(int row, int col)
{
    if (row < 0 || row >= vertexWidth || col < 0 || col >= vertexHeight)
        return -1;

    return row * vertexWidth + col;
}

/**
 * Computes the amount to perturb the height of the vertex currently being processed.
 * Feel free to modify this.
 *
 * @param depth The current recursion depth
 */
double Terrain::getPerturb()
{
    return (rand() % 200-100) / 100.0;
}

