#include "chunk.h"

const int Chunk::VERTEX_GRID_HEIGHT = 2;
const int Chunk::VERTEX_GRID_WIDTH = 2;
GLint Chunk::normalAttribLoc;
GLint Chunk::positionAttribLoc;

Chunk::Chunk(int level, glm::vec2 planePos, int numChunksX)
{
    m_level = level;
    m_planePos = planePos;
    m_numChunksX = numChunksX;
    m_children[0] = 0;
    m_children[1] = 0;
    m_children[2] = 0;
    m_children[3] = 0;
    m_heightData = 0;
    m_biomeData = 0;




}

Chunk::~Chunk() {
    delete m_children[0];
    delete m_children[1];
    delete m_children[2];
    delete m_children[3];
    delete[] m_heightData;
    delete[] m_biomeData;
}


void Chunk::draw() {
    /*glBindVertexArray(m_vaoID);
    for (int row = 0; row < VERTEX_GRID_HEIGHT; row++){
        glDrawArrays(GL_TRIANGLE_STRIP, (VERTEX_GRID_WIDTH+1)*2*row, (VERTEX_GRID_WIDTH+1)*2);
    }

    glBindVertexArray(0);
    */

    glBindVertexArray(m_vaoID);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}

void Chunk::drawRecursive(glm::vec3 cameraPos, float thetaWidth, float thetaHeight, int level) {\

    if(this->m_level <= level && isVisible(cameraPos, thetaWidth, thetaHeight)) {
        bool allChildrenExist = m_children[0] != 0 && m_children[1] != 0
                && m_children[2] != 0 && m_children[3] != 0;
        if(allChildrenExist) {
            for(int i = 0; i < 4; i++) {
                m_children[i]->drawRecursive(cameraPos, thetaWidth, thetaHeight, level);
            }
        } else {
            draw();
        }
    }


}

void Chunk::update(glm::vec3 cameraPos, float thetaWidth, float thetaHeight, int level) {
    if(m_level +1 <= level) {
        glm::vec2 childSize = glm::vec2(1.f/(2*m_numChunksX), 1.f/(2*m_numChunksX));
        for(int i = 0; i < 4; i++) {
            if(m_children[i] == 0) {
                int quadX = i%2;
                int quadY = i/2;
                glm::vec2 quadOffset = glm::vec2(quadX, quadY);
                glm::vec2 childPlanePos = quadOffset*childSize + m_planePos;
                m_children[i] = new Chunk(m_level + 1,  childPlanePos, 2*m_numChunksX);
                m_children[i]->generate(m_heightData, m_biomeData, i);
                m_children[i]->update(cameraPos, thetaWidth, thetaHeight, level);
            }
        }
    }
}


void Chunk::generate(float *parentHeightData, float *parentBiomeData, int quadrant) {
    assert(m_heightData == 0);
    assert(m_biomeData == 0);
    this->m_heightData = new float[(VERTEX_GRID_WIDTH+1)*(VERTEX_GRID_HEIGHT+1)];
    this->m_biomeData = new float[(VERTEX_GRID_WIDTH+1)*(VERTEX_GRID_HEIGHT+1)];

    for(int i = 0; i < VERTEX_GRID_WIDTH*(VERTEX_GRID_WIDTH+1); i++) {
        m_heightData[i] = 0;
    }


    int quadX = quadrant%2;
    int quadY = quadrant/2;
    int xOffset = quadX*VERTEX_GRID_WIDTH/2;
    int yOffset = quadY*VERTEX_GRID_HEIGHT/2;

    for(int row = 0; row < VERTEX_GRID_HEIGHT/2 + 1; row++) {
        for(int col = 0; col < VERTEX_GRID_WIDTH/2 + 1; col++) {
            m_heightData[(row*2)*(VERTEX_GRID_WIDTH+1) + (col*2)] =
                    parentHeightData[(row+yOffset)*(VERTEX_GRID_WIDTH+1) + col+xOffset];
        }
    }


    for(int row = 0; row < VERTEX_GRID_HEIGHT/2; row++) {
        for(int col = 0; col < VERTEX_GRID_WIDTH/2; col++) {
            glm::vec2 topLeft = glm::vec2(2*col, 2*row);
            glm::vec2 bottomRight = glm::vec2(2*(col+1), 2*(row+1));
            subdivideSquare(topLeft, bottomRight);
        }
    }

    initGL();
}

void Chunk::generate() {

    float *flatHeightData = new float[(VERTEX_GRID_WIDTH+1)*(VERTEX_GRID_HEIGHT+1)];
    for(int i = 0; i < (VERTEX_GRID_WIDTH+1)*(VERTEX_GRID_HEIGHT+1); i++) {
        flatHeightData[i] = 0;
    }
    generate(flatHeightData, flatHeightData, 0);

    delete[] flatHeightData;

}

void Chunk::subdivideSquare(glm::vec2 topleft, glm::vec2 botright)
{
    // TL--TM--TR    +---> x
    // |   |   |     |
    // ML--MM--MR    V
    // |   |   |     y
    // BL--BM--BR



    // Corner coordinates (in the grid space [x,y])
    glm::vec2 TL = glm::vec2(topleft.x, topleft.y);
    glm::vec2 TR = glm::vec2(botright.x, topleft.y);
    glm::vec2 BL = glm::vec2(topleft.x, botright.y);
    glm::vec2 BR = glm::vec2(botright.x, botright.y);



    // Corner vertices on the terrain (in the grid space [x,y,z])
    float hTL = m_heightData[getIndex(TL)];
    float hTR = m_heightData[getIndex(TR)];
    float hBL = m_heightData[getIndex(BL)];
    float hBR = m_heightData[getIndex(BR)];



    glm::vec2 TM = (TL + TR)*.5f;
    glm::vec2 RM = (TR + BR)*.5f;
    glm::vec2 BM = (BL + BR)*.5f;
    glm::vec2 LM = (TL + BL)*.5f;
    glm::vec2 MM = (TL + TR + BL + BR)*.25f;

    m_heightData[getIndex(TM)] = (hTL + hTR)*.5f;
    m_heightData[getIndex(RM)] = (hTR + hBR)*.5f;
    m_heightData[getIndex(BM)] = (hBL + hBR)*.5f;
    m_heightData[getIndex(LM)] = (hTL + hBL)*.5f;
    m_heightData[getIndex(MM)] = ((hTL + hTR + hBL + hBR)*.25f) + getPerturb();

}

/**
 * Initializes all GL components (assumes the height data has already been calculated
 *
 * @brief Chunk::initGL
 */
void Chunk::initGL()
{
/*
    glm::vec3 normals[(VERTEX_GRID_WIDTH+1)*(VERTEX_GRID_HEIGHT+1)];
    glm::vec3 vertices[(VERTEX_GRID_WIDTH+1)*(VERTEX_GRID_HEIGHT+1)];

    populateVertices(vertices);
    populateNormals(vertices, normals);

    int test1, test2;

    GLfloat *vertexBufferData = new GLfloat[12*(VERTEX_GRID_WIDTH+1)*(VERTEX_GRID_HEIGHT)];
    for(int i = 0; i < VERTEX_GRID_HEIGHT; i++) {
        for(int j = 0; j < VERTEX_GRID_WIDTH + 1; j++) {
            int bufferIndex = i*(VERTEX_GRID_WIDTH+1) + j;
            test1 = getIndex(j,i);
            test2 = getIndex(j,i+1);
            vertexBufferData[12*bufferIndex] = vertices[getIndex(j,i)].x;
            vertexBufferData[12*bufferIndex+1] = vertices[getIndex(j,i)].y;
            vertexBufferData[12*bufferIndex+2] = vertices[getIndex(j,i)].z;
            vertexBufferData[12*bufferIndex+3] = normals[getIndex(j,i)].x;
            vertexBufferData[12*bufferIndex+4] = normals[getIndex(j,i)].y;
            vertexBufferData[12*bufferIndex+5] = normals[getIndex(j,i)].z;

            vertexBufferData[12*bufferIndex+6] = vertices[getIndex(j,i+1)].x;
            vertexBufferData[12*bufferIndex+7] = vertices[getIndex(j,i+1)].y;
            vertexBufferData[12*bufferIndex+8] = vertices[getIndex(j,i+1)].z;
            vertexBufferData[12*bufferIndex+9] = normals[getIndex(j,i+1)].x;
            vertexBufferData[12*bufferIndex+10] = normals[getIndex(j,i+1)].y;
            vertexBufferData[12*bufferIndex+11] = normals[getIndex(j,i+1)].z;

            int k = 10;
        }
    }
*/

    GLfloat vertexBufferData[] = {
        0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 1,
        0, 1, 0, 0, 0, 1,
        1, 1, 0, 0, 0, 1,

    };

    // VAO init
    glGenVertexArrays(1, &m_vaoID);
    glBindVertexArray(m_vaoID);



    // Buffer init
    GLuint vboID;
    glGenBuffers(1, &vboID);
    glBindBuffer(GL_ARRAY_BUFFER, vboID);


    // @TODO: Pass your data to the GPU and specify how the attributes are organized. Recall
    //        that the relevant functions are glBufferData, glEnableVertexAttribArray, and
    //        glVertexAttribPointer.

    // Give our vertices to OpenGL.
    //glBufferData(GL_ARRAY_BUFFER, 12*((VERTEX_GRID_WIDTH+1)*(VERTEX_GRID_HEIGHT))*sizeof(GLfloat), vertexBufferData, GL_STATIC_DRAW);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);

    // Expose vertices to shader
    glEnableVertexAttribArray(positionAttribLoc);
    glVertexAttribPointer(
       positionAttribLoc,
       3,                  // num vertices per element (3 for triangle)
       GL_FLOAT,           // type
       GL_FALSE,           // normalized?
       0,                  // stride
       (void*)0            // array buffer offset
    );

    glEnableVertexAttribArray(normalAttribLoc);
    glVertexAttribPointer(
       normalAttribLoc,
       3,                  // num vertices per element (3 for triangle)
       GL_FLOAT,           // type
       GL_FALSE,            // normalized?
       0,                  // stride
       (void*)(3*sizeof(GLfloat))            // array buffer offset
    );





    // Clean up and unbind.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


}


/**
 * Converts a grid coordinate (row, column) to an index into a 1-dimensional array.
 * Can be used to index into m_terrain or m_normalMap.
 * Returns -1 if the grid coordinate entered is not valid.
 */
inline int Chunk::getIndex(const glm::vec2 &c)
{
    assert(c.x - (int)c.x < .001);
    return getIndex(c.x, c.y);
}

/**
 * Converts a grid coordinate (row, column) to an index into a 1-dimensional array.
 * Can be used to index into m_terrain or m_normalMap.
 * Returns -1 if the grid coordinate entered is not valid.
 */
inline int Chunk::getIndex(int col, int row)
{
    if (row < 0 || row >= VERTEX_GRID_WIDTH + 1 || col < 0 || col >= VERTEX_GRID_HEIGHT+1)
        return -1;

    return row * (VERTEX_GRID_WIDTH+1) + col;
}

/**
 * Computes the amount to perturb the height of the vertex currently being processed.
 * Feel free to modify this.
 *
 * @param depth The current recursion depth
 */
double Chunk::getPerturb()
{
    return (rand() % 200-100) / 100.0;
}

void Chunk::populateVertices(glm::vec3 *verticesOut) {

    assert(m_planePos.x >= 0 && m_planePos.x < 1 && m_planePos.y >= 0 && m_planePos.y < 1);

    float r = .5f;
    for(int i = 0; i < VERTEX_GRID_HEIGHT+1; i++) {
        for(int j = 0; j < VERTEX_GRID_WIDTH+1; j++) {

            verticesOut[i*(VERTEX_GRID_WIDTH+1) + j] =
                    glm::vec3(((float)j)/VERTEX_GRID_WIDTH,
                              ((float)i)/VERTEX_GRID_HEIGHT,
                              m_heightData[i*(VERTEX_GRID_WIDTH+1) + j]);

            /*
            int detailX = VERTEX_GRID_WIDTH*m_numChunksX;
            int detailY = VERTEX_GRID_HEIGHT*m_numChunksX;
            float theta = 2*j*M_PI/detailX + 2*M_PI*m_planePos.x;
            float phi = -M_PI + i*M_PI/detailY + M_PI*m_planePos.y;
            glm::vec3 p = glm::vec3(r*sin(phi)*cos(theta), r*cos(phi), r*sin(phi)*sin(theta));

            glm::vec3 n = glm::normalize(p);

            float displacement = 1.f/glm::max(VERTEX_GRID_WIDTH, VERTEX_GRID_HEIGHT); //???????????
            p = p + n*displacement;
            verticesOut[i*VERTEX_GRID_WIDTH + j] = p;
            */

        }
    }
}

void Chunk::populateNormals(glm::vec3 *verticesIn, glm::vec3 *normalsOut) {
    // For each vertex in the 2D grid...
    for (int row = 0; row < VERTEX_GRID_HEIGHT+1; row++) {
        for (int column = 0; column < VERTEX_GRID_WIDTH+1; column++) {
            // 2D coordinate of the vertex on the terrain grid.
            const glm::vec2 gridPosition(column, row);

            // Index into the 1D position and normal arrays.
            const int terrainIndex = getIndex(gridPosition);

            assert(terrainIndex >=0);

            // 3D position of the vertex.
            const glm::vec3 &vertexPosition = verticesIn[terrainIndex];

            // Get the neighbors of the vertex at (a,b).
            const QList<glm::vec3*>& neighbors = getSurroundingVertices(gridPosition, verticesIn);
            int numNeighbors = neighbors.size();

            // @TODO: Compute vectors from vertexPosition to each neighbor.
            glm::vec3 *vertToNeighbors = new glm::vec3[numNeighbors];
            for(int i = 0; i < numNeighbors; i++) {
                vertToNeighbors[i] = *neighbors.at(i) - vertexPosition;
            }


            // @TODO: Compute cross products for each neighbor.
            glm::vec3 *normals = new glm::vec3[numNeighbors];
            for(int i = 0; i < numNeighbors; i++) {
                normals[i] = glm::cross(vertToNeighbors[i], vertToNeighbors[(i+1)%numNeighbors]);
            }



            // @TODO: Compute the vertex normal and store in m_normalMap.
            glm::vec3 sum = glm::vec3(0,0,0);
            for(int i = 0; i < numNeighbors; i++) {
                sum = normals[i] + sum;
            }
            normalsOut[terrainIndex] = glm::vec3(0,0,0);//glm::normalize(sum);

            delete[] vertToNeighbors;
            delete[] normals;
        }
    }
}

QList<glm::vec3*> Chunk::getSurroundingVertices(const glm::vec2 &coordinate, glm::vec3 *vertices)
{
    // List of [x,y] grid offsets to find the neighbors of a vertex. i.e. (0,-1), (1,-1), etc.
    int offsets[] = {0,-1,  +1,-1,  +1,0,  +1,+1,  0,+1,  -1,+1,  -1,0,  -1,-1};

    QList<glm::vec3*> vecs;
    for (int i = 0; i < 8; i++)
    {
        int index = getIndex(coordinate + glm::vec2(offsets[2*i], offsets[2*i+1]));
        if (index != -1)
            vecs.push_back(& vertices[index]);
    }

    return vecs;
}

bool Chunk::isVisible(glm::vec3 cameraPos, float thetaWidth, float thetaHeight) {
    return true;
}

