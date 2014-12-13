#include "chunk.h"
#include "PerlinNoise.h"


Chunk::Chunk(int level, glm::vec2 planePos, int numChunksX, GLint shader, GLint shadowShader)
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
    m_shader = shader;
    m_shadowShader = m_shadowShader;
}

Chunk::~Chunk() {
    delete m_children[0];
    delete m_children[1];
    delete m_children[2];
    delete m_children[3];
    delete[] m_heightData;
    delete[] m_biomeData;
}


void Chunk::draw(GLint shader) {
    glUseProgram(shader);



    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBindVertexArray(m_vaoID);
    for (int row = 0; row < VERTEX_GRID_WIDTH; row++){
        glDrawArrays(GL_TRIANGLE_STRIP, (VERTEX_GRID_WIDTH+1)*2*row, (VERTEX_GRID_WIDTH+1)*2);
    }

    /*glm::vec3 normals[(VERTEX_GRID_WIDTH+1)*(VERTEX_GRID_HEIGHT+1)];
    glm::vec3 vertices[(VERTEX_GRID_WIDTH+1)*(VERTEX_GRID_HEIGHT+1)];

    populateVertices(vertices);
    populateNormals(vertices, normals);

    drawNormals(vertices, normals);
*/
    glBindVertexArray(0);

    glUseProgram(0);


}

void Chunk::drawRecursive(glm::vec3 cameraPos, int level, GLint shader) {

    if(this->isInView(cameraPos, 0.f)) {
        m_biomeData[0] = .5f;
    } else {
        m_biomeData[0] = 0.f;
    }


    bool allChildrenExist = m_children[0] != 0 && m_children[1] != 0
            && m_children[2] != 0 && m_children[3] != 0;
    bool tileIsVisible = isInView(cameraPos, 1000000.f);

    if(!tileIsVisible) {
        bool debug = isInView(cameraPos, 1000000.f);
    }

    if(m_level > level || !tileIsVisible) {
        return;
    }

    if(!allChildrenExist || m_level == level ) {
        draw(shader);
        return;
    }

    for(int i = 0; i < 4; i++) {
        m_children[i]->drawRecursive(cameraPos, level, shader);
    }

}

void Chunk::update(glm::vec3 cameraPos, int level) {


    if(m_level +1 <= level && isInView(cameraPos, 0/*.01f*pow(.5,level)*/)) {
        glm::vec2 childSize = glm::vec2(1.f/(2*m_numChunksX), 1.f/(2*m_numChunksX));
        for(int i = 0; i < 4; i++) {
            if(m_children[i] == 0) {
                int quadX = i%2;
                int quadY = i/2;
                glm::vec2 quadOffset = glm::vec2(quadX, quadY);
                glm::vec2 childPlanePos = quadOffset*childSize + m_planePos;

                m_children[i] = new Chunk(m_level + 1,  childPlanePos, 2*m_numChunksX, m_shader, m_shadowShader);
                m_children[i]->generate(m_heightData, m_biomeData, i);

            }
            m_children[i]->update(cameraPos, level);
        }
    }
}


void Chunk::generate(float *parentHeightData, float *parentBiomeData, int quadrant) {
    assert(m_heightData == 0);
    assert(m_biomeData == 0);
    this->m_heightData = new float[(VERTEX_GRID_WIDTH+1)*(VERTEX_GRID_WIDTH+1)];
    this->m_biomeData = new float[(VERTEX_GRID_WIDTH+1)*(VERTEX_GRID_WIDTH+1)];

    for(int i = 0; i < VERTEX_GRID_WIDTH*(VERTEX_GRID_WIDTH+1); i++) {
        m_heightData[i] = 0;
        m_biomeData[i] = 0;
    }


    int quadX = quadrant%2;
    int quadY = quadrant/2;
    int xOffset = quadX*VERTEX_GRID_WIDTH/2;
    int yOffset = quadY*VERTEX_GRID_WIDTH/2;

    for(int row = 0; row < VERTEX_GRID_WIDTH/2 + 1; row++) {
        for(int col = 0; col < VERTEX_GRID_WIDTH/2 + 1; col++) {
            m_heightData[(row*2)*(VERTEX_GRID_WIDTH+1) + (col*2)] =
                    parentHeightData[(row+yOffset)*(VERTEX_GRID_WIDTH+1) + col+xOffset];
        }
    }


    for(int row = 0; row < VERTEX_GRID_WIDTH/2; row++) {
        for(int col = 0; col < VERTEX_GRID_WIDTH/2; col++) {
            glm::vec2 topLeft = glm::vec2(2*col, 2*row);
            glm::vec2 bottomRight = glm::vec2(2*(col+1), 2*(row+1));
            subdivideSquare(topLeft, bottomRight, 1);
        }
    }

    initGL();
}

void Chunk::generateRoot() {

    m_heightData = new float[(VERTEX_GRID_WIDTH+1)*(VERTEX_GRID_WIDTH+1)];
    m_biomeData = new float[(VERTEX_GRID_WIDTH+1)*(VERTEX_GRID_WIDTH+1)];

    //TODO: BAD
    for(int i = 0; i < VERTEX_GRID_WIDTH*(VERTEX_GRID_WIDTH+1); i++) {
        m_heightData[i] = 0;
        m_biomeData[i] = 0;
    }


    m_heightData[getIndex(0,0)] = 0;
    m_heightData[getIndex(0,VERTEX_GRID_WIDTH)] = 0;
    m_heightData[getIndex(VERTEX_GRID_WIDTH,0)] = 0;
    m_heightData[getIndex(VERTEX_GRID_WIDTH,VERTEX_GRID_WIDTH)];


        //check to make sure we don't get floating point errors. I don't trust floats!
        float val = (float)((int)(log(VERTEX_GRID_WIDTH)/log(2)))- log(VERTEX_GRID_WIDTH)/log(2);
        assert(val < .00000001f);

      /*  int pWidth = 8;
        float *sparcePerlin = new float[(pWidth+1)*(pWidth+1)];

        PerlinNoise pn = PerlinNoise();
        pn.noise(sparcePerlin, pWidth+1, 1, .5f, .2, 1.f/8.f);

        for(int i = 0; i < pWidth+1; i++) {
            for(int j = 0; j < pWidth+1; j++) {
                m_heightData[getIndex(VERTEX_GRID_WIDTH/pWidth*j, VERTEX_GRID_WIDTH/pWidth*i)] = sparcePerlin[(pWidth+1)*i + j];
            }
        }

        delete[] sparcePerlin;


        for(int i = 0; i < 8; i++) {
            for(int j = 0; j < 8; j++) {
                glm::vec2 topLeft = glm::vec2(j*8, i*8);
                glm::vec2 botright = glm::vec2((j+1)*8, (i+1)*8);
                subdivideSquare(topLeft, botright, 3);

            }
        }*/


        int depth = (int)(log(VERTEX_GRID_WIDTH)/log(2));
        subdivideSquare(glm::vec2(0,0), glm::vec2(VERTEX_GRID_WIDTH, VERTEX_GRID_WIDTH),depth);


        initGL();

}



void Chunk::subdivideSquare(glm::vec2 topleft, glm::vec2 botright, int depth)
{
    if(depth <= 0) return;

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
    glm::vec2 MR = (TR + BR)*.5f;
    glm::vec2 BM = (BL + BR)*.5f;
    glm::vec2 ML = (TL + BL)*.5f;
    glm::vec2 MM = (TL + TR + BL + BR)*.25f;

    float currGridWidth =  VERTEX_GRID_WIDTH/(botright.x - topleft.x);
    int globalDepth = 1+log(currGridWidth*m_numChunksX)/log(2);

    m_heightData[getIndex(TM)] = (hTL + hTR)*.5f;
    m_heightData[getIndex(MR)] = (hTR + hBR)*.5f;
    m_heightData[getIndex(BM)] = (hBL + hBR)*.5f;
    m_heightData[getIndex(ML)] = (hTL + hBL)*.5f;
    m_heightData[getIndex(MM)] = glm::clamp(((hTL + hTR + hBL + hBR)*.25f) + getPerturb(globalDepth, topleft.y),
                                            -MAX_MOUNTAIN_HEIGHT, MAX_MOUNTAIN_HEIGHT);

    subdivideSquare(TL, MM, depth-1);
    subdivideSquare(ML, BM, depth-1);
    subdivideSquare(TM, MR, depth-1);
    subdivideSquare(MM, BR, depth-1);
}

void Chunk::subdivideSquareDiamond(glm::vec2 topleft, glm::vec2 botright, int depth) {

    int currWidth =1;
    for (int currDepth = 0; currDepth < depth; currDepth++) {
        float squareLength = glm::abs(botright.x - topleft.x)/(float)currWidth;
        for(int i = 0; i < currWidth; i++) {
            for(int j = 0; j < currWidth; j++) {

                if(currWidth == 2) {
                    int k = 12;
                }
                subdivideSquareDiamond1(glm::vec2(squareLength*j,squareLength*i),
                                        glm::vec2(squareLength*(j+1), squareLength*(i+1)));
            }
        }
        for(int i = 0; i < currWidth; i++) {
            for(int j = 0; j < currWidth; j++) {
                subdivideSquareDiamond2(glm::vec2(squareLength*j,squareLength*i),
                                        glm::vec2(squareLength*(j+1), squareLength*(i+1)));
            }
        }
        currWidth*=2;

    }



}

void Chunk::subdivideSquareDiamond1(glm::vec2 topleft, glm::vec2 botright){

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

    glm::vec2 MM = (TL + TR + BL + BR)*.25f;

    float currGridWidth =  VERTEX_GRID_WIDTH/(botright.x - topleft.x);
    int globalDepth = 1+log(currGridWidth*m_numChunksX)/log(2);

    m_heightData[getIndex(MM)] = glm::clamp(((hTL + hTR + hBL + hBR)*.25f) + getPerturb(globalDepth, MM.y),
                                            -MAX_MOUNTAIN_HEIGHT, MAX_MOUNTAIN_HEIGHT);
}

void Chunk::subdivideSquareDiamond2(glm::vec2 topleft, glm::vec2 botright){

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
    glm::vec2 MM = (TL + TR + BL + BR)*.25f;


    glm::vec2 TM = (TL + TR)*.5f;
    glm::vec2 MR = (TR + BR)*.5f;
    glm::vec2 BM = (BL + BR)*.5f;
    glm::vec2 ML = (TL + BL)*.5f;
    glm::vec2 TTM = TM + (TM - MM);
    glm::vec2 MLL = ML + (ML - MM);
    glm::vec2 BBM = BM + (BM - MM);
    glm::vec2 MRR = MR + (MR - MM);

    float hTL = m_heightData[getIndex(TL)];
    float hTR = m_heightData[getIndex(TR)];
    float hBL = m_heightData[getIndex(BL)];
    float hBR = m_heightData[getIndex(BR)];
    float hMM = m_heightData[getIndex(MM)];
    float hTTM = getIndex(TTM) != -1 ? m_heightData[getIndex(TTM)] : (hTL + hMM + hTR)/3.f;
    float hMLL = getIndex(MLL) != -1 ? m_heightData[getIndex(MLL)] : (hTL + hMM + hBL)/3.f;
    float hBBM = getIndex(BBM) != -1 ? m_heightData[getIndex(BBM)] : (hBL + hMM + hBR)/3.f;
    float hMRR = getIndex(MRR) != -1 ? m_heightData[getIndex(MRR)] : (hTR + hMM + hBR)/3.f;




    float currGridWidth =  VERTEX_GRID_WIDTH/(botright.x - topleft.x);
    int globalDepth = 1+log(currGridWidth*m_numChunksX)/log(2);

    m_heightData[getIndex(TM)] = glm::clamp((hTL + hTR + hMM + hTTM)*.25f + getPerturb(globalDepth, TM.y),
                                            -MAX_MOUNTAIN_HEIGHT, MAX_MOUNTAIN_HEIGHT);
    m_heightData[getIndex(MR)] = glm::clamp((hTR + hBR + hMM + hMRR)*.25f + getPerturb(globalDepth, MR.y),
                                            -MAX_MOUNTAIN_HEIGHT, MAX_MOUNTAIN_HEIGHT);
    m_heightData[getIndex(BM)] = glm::clamp((hBL + hBR + hMM + hBBM)*.25f + getPerturb(globalDepth, BM.y),
                                            -MAX_MOUNTAIN_HEIGHT, MAX_MOUNTAIN_HEIGHT);
    m_heightData[getIndex(ML)] = glm::clamp((hTL + hBL + hMM + hMLL)*.25f + getPerturb(globalDepth, ML.y),
                                          -MAX_MOUNTAIN_HEIGHT, MAX_MOUNTAIN_HEIGHT);

    //wrapping
    if(botright.x >= VERTEX_GRID_WIDTH) {
        float otherside = m_heightData[getIndex(0,(int)MRR.y)];
        m_heightData[getIndex(MR)] = otherside;
    }


}

/**
 * Initializes all GL components (assumes the height data has already been calculated
 *
 * @brief Chunk::initGL
 */
void Chunk::initGL()
{

    int numFloats = 10;

    GLuint normalAttribLoc = glGetAttribLocation(m_shader, "normal");
    GLuint positionAttribLoc = glGetAttribLocation(m_shader, "position");
    GLuint texCoordAttribLoc = glGetAttribLocation(m_shader, "texCoord");
    GLuint dataAttribLoc = glGetAttribLocation(m_shader, "data");

    glm::vec3 *normals = new glm::vec3[(VERTEX_GRID_WIDTH+1)*(VERTEX_GRID_WIDTH+1)];
    glm::vec3 *vertices = new glm::vec3[(VERTEX_GRID_WIDTH+1)*(VERTEX_GRID_WIDTH+1)];

    populateVertices(vertices);
    populateNormals(vertices, normals);


    GLfloat *vertexBufferData = new GLfloat[2*numFloats*(VERTEX_GRID_WIDTH+1)*(VERTEX_GRID_WIDTH)];
    for(int i = 0; i < VERTEX_GRID_WIDTH; i++) {
        for(int j = 0; j < VERTEX_GRID_WIDTH + 1; j++) {
            int bufferIndex = i*(VERTEX_GRID_WIDTH+1) + j;

            vertexBufferData[2*numFloats*bufferIndex] = vertices[getIndex(j,i)].x;
            vertexBufferData[2*numFloats*bufferIndex+1] = vertices[getIndex(j,i)].y;
            vertexBufferData[2*numFloats*bufferIndex+2] = vertices[getIndex(j,i)].z;
            vertexBufferData[2*numFloats*bufferIndex+3] = normals[getIndex(j,i)].x;
            vertexBufferData[2*numFloats*bufferIndex+4] = normals[getIndex(j,i)].y;
            vertexBufferData[2*numFloats*bufferIndex+5] = normals[getIndex(j,i)].z;
            vertexBufferData[2*numFloats*bufferIndex+6] = (float)j/VERTEX_GRID_WIDTH;
            vertexBufferData[2*numFloats*bufferIndex+7] = (float)i/VERTEX_GRID_WIDTH;
            vertexBufferData[2*numFloats*bufferIndex+8] = m_heightData[getIndex(j,i)];
            vertexBufferData[2*numFloats*bufferIndex+9] = m_biomeData[getIndex(j,i)];

            vertexBufferData[2*numFloats*bufferIndex+10] = vertices[getIndex(j,i+1)].x;
            vertexBufferData[2*numFloats*bufferIndex+11] = vertices[getIndex(j,i+1)].y;
            vertexBufferData[2*numFloats*bufferIndex+12] = vertices[getIndex(j,i+1)].z;
            vertexBufferData[2*numFloats*bufferIndex+13] = normals[getIndex(j,i+1)].x;
            vertexBufferData[2*numFloats*bufferIndex+14] = normals[getIndex(j,i+1)].y;
            vertexBufferData[2*numFloats*bufferIndex+15] = normals[getIndex(j,i+1)].z;
            vertexBufferData[2*numFloats*bufferIndex+16] = (float)j/VERTEX_GRID_WIDTH;
            vertexBufferData[2*numFloats*bufferIndex+17] = (float)(i+1)/VERTEX_GRID_WIDTH;
            vertexBufferData[2*numFloats*bufferIndex+18] = m_heightData[getIndex(j,i+1)];
            vertexBufferData[2*numFloats*bufferIndex+19] = m_biomeData[getIndex(j,i+1)];

        }
    }



/*
    GLfloat vertexBufferData[] = {
        0.f, 0.f, 0.f, 0.f, 0.f, 1.f,
        1.f, 0.f, 0.f, 0.f, 0.f, 1.f,
        0.f, 1.f, 0.f, 0.f, 0.f, 1.f,
        1.f, 1.f, 0.f, 0.f, 0.f, 1.f,
        0.f, 10.f, 0.f, 0.f, 0.f, 1.f,
        1.f, 10.f, 0.f, 0.f, 0.f, 1.f,

    };
*/
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
    glBufferData(GL_ARRAY_BUFFER, 2*numFloats*((VERTEX_GRID_WIDTH+1)*(VERTEX_GRID_WIDTH))*sizeof(GLfloat), vertexBufferData, GL_STATIC_DRAW);


    // Expose vertices to shader
    glEnableVertexAttribArray(positionAttribLoc);
    glVertexAttribPointer(
       positionAttribLoc,
       3,                  // num vertices per element (3 for triangle)
       GL_FLOAT,           // type
       GL_FALSE,           // normalized?
       numFloats*sizeof(GLfloat),                  // stride
       (void*)0            // array buffer offset
    );

   glEnableVertexAttribArray(normalAttribLoc);
    glVertexAttribPointer(
       normalAttribLoc,
       3,                  // num vertices per element (3 for triangle)
       GL_FLOAT,           // type
       GL_FALSE,            // normalized?
       numFloats*sizeof(GLfloat),                  // stride
       (void*)(3*sizeof(GLfloat))            // array buffer offset
    );

    glEnableVertexAttribArray(texCoordAttribLoc);
     glVertexAttribPointer(
        texCoordAttribLoc,
        2,                  // num vertices per element (3 for triangle)
        GL_FLOAT,           // type
        GL_FALSE,            // normalized?
        numFloats*sizeof(GLfloat),                  // stride
        (void*)(6*sizeof(GLfloat))            // array buffer offset
     );

     glEnableVertexAttribArray(dataAttribLoc);
      glVertexAttribPointer(
         dataAttribLoc,
         2,                  // num vertices per element (3 for triangle)
         GL_FLOAT,           // type
         GL_FALSE,            // normalized?
         numFloats*sizeof(GLfloat),                  // stride
         (void*)(8*sizeof(GLfloat))            // array buffer offset
      );



    // Clean up and unbind.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    delete[] vertices;
    delete[] normals;
    delete[] vertexBufferData;


}


/**
 * Converts a grid coordinate (row, column) to an index into a 1-dimensional array.
 * Can be used to index into m_terrain or m_normalMap.
 * Returns -1 if the grid coordinate entered is not valid.
 */
inline int Chunk::getIndex(const glm::vec2 &c)
{
    if(glm::abs(c.x - (float)((int)c.x)) >= .001) {
        int k = (int)c.x;
        float stuff = glm::abs(c.x - (float)((int)c.x));

        int ba = 123;
    }

    assert(glm::abs(c.x - (float)((int)c.x)) < .001);
    return getIndex(c.x, c.y);
}

/**
 * Converts a grid coordinate (row, column) to an index into a 1-dimensional array.
 * Can be used to index into m_terrain or m_normalMap.
 * Returns -1 if the grid coordinate entered is not valid.
 */
inline int Chunk::getIndex(int col, int row)
{
    if (row < 0 || row >= VERTEX_GRID_WIDTH + 1 || col < 0 || col >= VERTEX_GRID_WIDTH+1)
        return -1;

    return row * (VERTEX_GRID_WIDTH+1) + col;
}


float Chunk::getPerturb(int cur_depth, float height) {
    float scale = 2*glm::abs(height/(VERTEX_GRID_WIDTH) - .5f);
    scale = 1.f - pow(scale, .5f);
    return scale*getPerturb(cur_depth);
}

/**
 * Computes the amount to perturb the height of the vertex currently being processed.
 * Feel free to modify this.
 *
 * @param depth The current recursion depth
 */
float Chunk::getPerturb(int cur_depth)
{
    //float scale = ROUGHNESS *pow(1 - (double)cur_depth / MAX_DEPTH, DECAY);
    float scale = ROUGHNESS*pow(1.f/DECAY,(double) cur_depth);
    float out =  scale * ((rand() % 200-100) / 100.0);

    return out;
}

void Chunk::populateVertices(glm::vec3 *verticesOut) {

    assert(m_planePos.x >= 0 && m_planePos.x < 1 && m_planePos.y >= 0 && m_planePos.y < 1);

    float r = .5f;
    for(int i = 0; i < VERTEX_GRID_WIDTH+1; i++) {
        for(int j = 0; j < VERTEX_GRID_WIDTH+1; j++) {

            //plane
            verticesOut[i*(VERTEX_GRID_WIDTH+1) + j] =
                    glm::vec3(m_planePos.x + ((float)j)/(VERTEX_GRID_WIDTH*m_numChunksX),
                              m_heightData[i*(VERTEX_GRID_WIDTH+1) + j],
                              m_planePos.y + ((float)i)/(VERTEX_GRID_WIDTH*m_numChunksX));

            //wraps to sphere


            glm::vec3 p = getPointOnSphere(glm::vec2(j,i));
            glm::vec3 n = glm::normalize(p);

            float displacement = glm::max(WATER_LEVEL, m_heightData[getIndex(j,i)]);
            p = p + n*displacement;
            verticesOut[getIndex(j,i)] = p;


        }
    }
}

void Chunk::populateNormals(glm::vec3 *verticesIn, glm::vec3 *normalsOut) {
    // For each vertex in the 2D grid...
    for (int row = 0; row < VERTEX_GRID_WIDTH+1; row++) {
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
                normals[i] = -glm::cross(vertToNeighbors[i], vertToNeighbors[(i+1)%numNeighbors]);
            }



            // @TODO: Compute the vertex normal and store in m_normalMap.
            glm::vec3 sum = glm::vec3(0,0,0);
            for(int i = 0; i < numNeighbors; i++) {
                sum = normals[i] + sum;
            }
            normalsOut[terrainIndex] = glm::normalize(sum);


            //smooth out seam
            for(int i = 0; i <= VERTEX_GRID_WIDTH; i++) {
                //glm::vec3 avg = glm::normalize(normals[getIndex(0,i)] + normals[getIndex(VERTEX_GRID_WIDTH, i)]);
                //normalsOut[getIndex(0,i)] = avg;
                //normalsOut[getIndex(VERTEX_GRID_WIDTH,i)] = avg;
            }

            delete[] vertToNeighbors;
            delete[] normals;
        }
    }
}

bool Chunk::isInView(glm::vec3 cameraLoc, float error) {
    isInView(cameraLoc, cameraLoc - glm::vec3(0,0,0), error);
}

bool Chunk::isInView(glm::vec3 cameraLoc, glm::vec3 dir, float error){
    if(m_level <= 1) {
        //allways show outer layers
        return true;
    }
    //ray plane intersection
    glm::vec3 p_0 = getPointOnSphere(glm::vec2(0,0));
    glm::vec3 p_1 = getPointOnSphere(glm::vec2(VERTEX_GRID_WIDTH, 0));
    glm::vec3 p_2 = getPointOnSphere(glm::vec2(0, VERTEX_GRID_WIDTH));
    glm::vec3 n = glm::normalize(glm::cross(p_1 - p_0, p_2 - p_0));
    glm::vec3 l = glm::normalize(dir);
    glm::vec3 l_0 = cameraLoc;
    float d = glm::dot(p_0 - l_0, n)/glm::dot(l,n);
    glm::vec3 planeLoc = l_0 + d*l;

    //if facing the wrong way
    if(glm::dot(n, dir) > 0) {
        //return false;
    }

    //check to see if point is within bounds;
    float width = glm::max(glm::length(p_0-p_1), glm::length(p_0-p_2));
    float x = glm::dot((planeLoc-p_0), glm::normalize(p_1-p_0));
    float y = glm::dot((planeLoc-p_0), glm::normalize(p_2-p_0));

    if(-error <= x && x <= width + error && -error <= y && y <= width + error) {
        return true;
    }
    return false;
}

glm::vec3 Chunk::getPointOnSphere(glm::vec2 coord) {
    float r = RADIUS;
    int detailX = VERTEX_GRID_WIDTH*m_numChunksX;
    int detailY = VERTEX_GRID_WIDTH*m_numChunksX;
    float theta = 2*coord.x*M_PI/detailX + 2*M_PI*m_planePos.x;
    float phi = -M_PI + coord.y*M_PI/detailY + M_PI*m_planePos.y;
    return glm::vec3(r*sin(phi)*cos(theta), r*cos(phi), r*sin(phi)*sin(theta));
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


void Chunk::drawNormals(glm::vec3 * vertices, glm::vec3 *normals){
    glColor3f(1,0,0);

    for (int row = 0; row < VERTEX_GRID_WIDTH+1; row++)
    {
        for (int column = 0; column < VERTEX_GRID_WIDTH; column++)
        {
            glBegin(GL_LINES);

            glm::vec3 curVert = vertices[getIndex(column, row)];
            glm::vec3 curNorm = normals[getIndex(column, row)];

            glNormal3f(curNorm.x, curNorm.y, curNorm.z);
            glVertex3f(curVert.x, curVert.y, curVert.z);
            glVertex3f(curVert.x +curNorm.x,
                       curVert.y + curNorm.y,
                       curVert.z + curNorm.z);

            glEnd();
        }
    }

}




