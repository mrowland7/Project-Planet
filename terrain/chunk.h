#ifndef CHUNK_H
#define CHUNK_H

#include <glm/glm.hpp>
#include "GL/glew.h"
#include "QList"

//a chunk of terrain on the sphere (determined by radial cuts of the sphere)
//represented by a square grid of vertex heights.
class Chunk
{

public:
    Chunk(int level, glm::vec2 planePos, int numChunksX);
    ~Chunk();


    static GLint positionAttribLoc;
    static GLint normalAttribLoc;

protected:


    void draw();
    void drawRecursive(glm::vec3 cameraPos, float thetaWidth, float thetaHeight, int m_level);

    void update(glm::vec3 cameraPos, float thetaWidth, float thetaHeight, int m_level);





private:

    void generate(float *parentHeightData, float *parentBiomeData, int quadrant);
    void subdivideSquare(glm::vec2 topleft, glm::vec2 botright);
    int getIndex(const glm::vec2 &c);
    int getIndex(int row, int col);
    double getPerturb();
    void initGL();
    void populateVertices(glm::vec3 *verticesOut);
    void populateNormals(glm::vec3 *verticesIn, glm::vec3 *normalsOut);
    QList<glm::vec3*> getSurroundingVertices(const glm::vec2 &coordinate, glm::vec3 *vertices);
    bool isVisible(glm::vec3 cameraPos, float thetaWidth, float thetaHeight);

    GLuint m_vaoID;
    GLuint m_vboID;
    int m_level;

    glm::vec2 m_planePos; //coordinate of the top left corner of this chunk on
                        //the unit plane (our sphere stretched out onto a plane using a Mercator projection)
    int m_numChunksX; //number of these size chunks that fit horizontally on the unit plane


    //number of vertices in square chunk
    static const int VERTEX_WIDTH;
    static const int VERTEX_HEIGHT;

    float *m_heightData; //heights of vertices (this is the important data)
    //float *m_normalData; //normals of the vertices
    float *m_biomeData; //specifies what type of biome each vertex belongs to

    Chunk *m_children[4]; //four, more detailed, children


};

 const int Chunk::VERTEX_HEIGHT = 8;
 const int Chunk::VERTEX_WIDTH = 8;
 GLint Chunk::normalAttribLoc = 0;
 GLint Chunk::positionAttribLoc = 0;

#endif // CHUNK_H
