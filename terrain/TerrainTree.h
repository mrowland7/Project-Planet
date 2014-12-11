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

    glm::mat4 getModel();
    void setModel(glm::mat4 model);

private:
    Chunk *m_root;
    int getLevel(glm::vec3 cameraPos, float thetaWidth, float thetaHeight);
    glm::mat4 m_model;
};

#endif // TERRAINTREE_H
