#ifndef CHUNK_H
#define CHUNK_H

#include <glm/glm.hpp>
#include "GL/glew.h"
#include "QList"
#include <iostream>

//a chunk of terrain on the sphere (determined by radial cuts of the sphere)
//represented by a square grid of vertex heights.
class Chunk
{

public:


    Chunk(int level, glm::vec2 planePos, int numChunksX, GLint shader, GLint shadowShader);
    ~Chunk();


    //CONSTANTS
    const int VERTEX_GRID_WIDTH = 32;//number squares in our vertex grid
    const float MAX_MOUNTAIN_HEIGHT = .1f;
    const float ROUGHNESS = 5.0f;
    //const int MAX_DEPTH = 9;
    const float DECAY = 2.0f;
    const float RADIUS = .5f;
    const float WATER_LEVEL = 0.f;

    void draw(GLint shader);
    void drawRecursive(glm::vec3 cameraPos, int m_level, GLint shader);

    void update(glm::vec3 cameraPos, int m_level);

    void generateRoot();







private:


    void generate(float *parentHeightData, float *parentBiomeData, int quadrant);
    void subdivideSquare(glm::vec2 topleft, glm::vec2 botright, int depth);
    void subdivideSquareDiamond(glm::vec2 topleft, glm::vec2 botright, int depth);
    void subdivideSquareDiamond1(glm::vec2 topleft, glm::vec2 botright);
    void subdivideSquareDiamond2(glm::vec2 topleft, glm::vec2 botright);
    int getIndex(const glm::vec2 &c);
    int getIndex(int col, int row);
    float getPerturb(int level);
    float getPerturb(int level, float height);
    void initGL();
    void populateVertices(glm::vec3 *verticesOut);
    void populateNormals(glm::vec3 *verticesIn, glm::vec3 *normalsOut);
    QList<glm::vec3*> getSurroundingVertices(const glm::vec2 &coordinate, glm::vec3 *vertices);
    bool isInView(glm::vec3 cameraLoc, float error);
    bool isInView(glm::vec3 cameraLoc, glm::vec3 dir, float error);
    glm::vec3 getPointOnSphere(glm::vec2 coord);

    GLuint m_vaoID;
    int m_level;

    glm::vec2 m_planePos; //coordinate of the top left corner of this chunk on
                        //the unit plane (our sphere stretched out onto a plane using a Mercator projection)
    int m_numChunksX; //number of these size chunks that fit horizontally on the unit plane


    float *m_heightData; //heights of vertices (this is the important data)
    //float *m_normalData; //normals of the vertices
    float *m_biomeData; //specifies what type of biome each vertex belongs to

    Chunk *m_children[4]; //four, more detailed, children

    GLint m_shader;
    GLint m_shadowShader;

    void drawNormals(glm::vec3 * vertices, glm::vec3 *normals);


    float perlin(float x, float y);


};



#endif // CHUNK_H
