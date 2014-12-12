#ifndef TERRAINTREE_H
#define TERRAINTREE_H

#include <glm/glm.hpp>
#include "chunk.h"



class TerrainTree
{
public:
    TerrainTree(GLint shader, GLint shadowShader);
    ~TerrainTree();
    void update(glm::vec3 cameraPos);
    void draw(glm::vec3 cameraPos, GLint shader);

    glm::mat4 getModel();
    void setModel(glm::mat4 model);
    float getHeight();
    glm::vec3 getLoc();


private:
    Chunk *m_root;
    int getLevel(glm::vec3 cameraPos);
    glm::mat4 m_model;

};

#endif // TERRAINTREE_H
