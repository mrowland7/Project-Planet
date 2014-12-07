#ifndef CHUNK_H
#define CHUNK_H

#include <glm/glm.hpp>

//a chunk of terrain on the sphere (determined by radial cuts of the sphere)
//represented by a square grid of vertex heights.
class Chunk
{

public:
    Chunk();
    ~Chunk();


protected:


    void draw();
    void drawRecursive(glm::vec3 cameraPos, float thetaWidth, float thetaHeight, int level);

    void update(glm::vec3 cameraPos, float thetaWidth, float thetaHeight, int level);



private:
    int level;

    void generate(float *parentHeightData, float *parentBiomeData, int quadrant);

    glm::vec2 planePos; //coordinate of the top left corner of this chunk on
                        //the unit plane (our sphere stretched out onto a plane using a Mercator projection)
    int chunksOnPlaneWidth; //number of these size chunks that fit horizontally on the unit plane
    int chunksOnPlaneHeight; // number of these size chunks that fit vertically on the unit plane


    //number of vertices in square chunk
    static const int vertexWidth = 8;
    static const int vertexHeight = 8;

    float *heightData; //heights of vertices (this is the important data)
    float *normalData; //normals of the vertices
    float *biomeData; //specifies what type of biome each vertex belongs to

    Chunk *children[4]; //four, more detailed, children


};

#endif // CHUNK_H
