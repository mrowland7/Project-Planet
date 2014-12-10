#ifndef TERRAINTREE_H
#define TERRAINTREE_H

#include <glm/glm.hpp>
#include "chunk.h"



class TerrainTree
{
public:
    TerrainTree(GLint shader);
    ~TerrainTree();
    void update(glm::vec3 cameraPos, float thetaWidth, float thetaHeight);
    void draw(glm::vec3 cameraPos, float thetaWidth, float thetaHeight);

private:
    Chunk *m_root;
    GLint m_shader;
    int getLevel(glm::vec3 cameraPos, float thetaWidth, float thetaHeight);
};

#endif // TERRAINTREE_H
