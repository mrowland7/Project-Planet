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


    //number squares in our vertex grid
    static const int VERTEX_GRID_WIDTH;
    static const int VERTEX_GRID_HEIGHT;

    static GLint positionAttribLoc;
    static GLint normalAttribLoc;

    void draw();
    void drawRecursive(glm::vec3 cameraPos, float thetaWidth, float thetaHeight, int m_level);

    void update(glm::vec3 cameraPos, float thetaWidth, float thetaHeight, int m_level);

    void generate();



private:

    void generate(float *parentHeightData, float *parentBiomeData, int quadrant);
    void subdivideSquare(glm::vec2 topleft, glm::vec2 botright);
    int getIndex(const glm::vec2 &c);
    int getIndex(int col, int row);
    double getPerturb();
    void initGL();
    void populateVertices(glm::vec3 *verticesOut);
    void populateNormals(glm::vec3 *verticesIn, glm::vec3 *normalsOut);
    QList<glm::vec3*> getSurroundingVertices(const glm::vec2 &coordinate, glm::vec3 *vertices);
    bool isVisible(glm::vec3 cameraPos, float thetaWidth, float thetaHeight);

    GLuint m_vaoID;
    int m_level;

    glm::vec2 m_planePos; //coordinate of the top left corner of this chunk on
                        //the unit plane (our sphere stretched out onto a plane using a Mercator projection)
    int m_numChunksX; //number of these size chunks that fit horizontally on the unit plane


    float *m_heightData; //heights of vertices (this is the important data)
    //float *m_normalData; //normals of the vertices
    float *m_biomeData; //specifies what type of biome each vertex belongs to

    Chunk *m_children[4]; //four, more detailed, children


};



#endif // CHUNK_H
