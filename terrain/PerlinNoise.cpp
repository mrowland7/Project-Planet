#include "PerlinNoise.h"

PerlinNoise::PerlinNoise()
{
    m_gridWidth = 64;
    m_grid = new glm::vec2[(m_gridWidth+1)*(m_gridWidth+1)];
    for(int i = 0; i < (m_gridWidth + 1)*(m_gridWidth + 1); i++) {
        float x = (rand() % 20000-10000) / 10000.0;
        float y = (rand() % 20000-10000) / 10000.0;
        m_grid[i] = glm::normalize(glm::vec2(x,y));
    }
}

PerlinNoise::~PerlinNoise() {
    delete[] m_grid;
}

float PerlinNoise::getNoise(float x, float y) {

    assert(x >= 0 && x < m_gridWidth && y >= 0 && y < m_gridWidth);

    int x0 = (int)x;
    int x1 = x0 + 1;
    int y0 = (int)y;
    int y1 = y0 + 1;

    float dx0 = x - x0;
    float dx1 = x - x1;
    float dy0 = y - y0;
    float dy1 = y - y1;

    float v0 = glm::dot(glm::vec2(dx0,dy0),m_grid[getIndex(x0,y0)]);
    float v1 = glm::dot(glm::vec2(dx1,dy0),m_grid[getIndex(x1,y0)]);
    float v2 = glm::dot(glm::vec2(dx0,dy1),m_grid[getIndex(x0,y1)]);
    float v3 = glm::dot(glm::vec2(dx1,dy1),m_grid[getIndex(x1,y1)]);

    float lx1 = lerp(v0,v1,dx0);
    float lx2 = lerp(v2,v3,dx0);
    return lerp(lx1, lx2, dy0);

}


inline int PerlinNoise::getIndex(int x, int y) {
    return x + y*(m_gridWidth + 1);
}

inline float PerlinNoise::lerp(float x1, float x2, float c) {
    return (1-c)*x1 + c*x2;
}

void PerlinNoise::makeOctave(float *outGrid, int width, float frequency, float amplitude) {
    float scale = ((float) m_gridWidth/width)*frequency;
    for(int i = 0; i < width; i++) {
        for(int j = 0; j < width; j++) {
            outGrid[i*width + j] = amplitude*getNoise(j*scale, i*scale);

        }

    }
}

void PerlinNoise::noise(float *outGrid, int width, int numOctaves, float persistence, float scale, float freq) {

    for(int k = 0; k < width*width; k++) {
        outGrid[k] = 0;
    }

    float *grid = new float[width*width];
    for(int i = 0; i < numOctaves; i++) {
        float frequency = freq*pow(2.0, (float)i);
        float amplitude = scale*pow(persistence, (float)i);
        makeOctave(grid, width, frequency, amplitude);
        for(int k = 0; k < width*width; k++) {
            outGrid[k] += grid[k];
        }
    }
}
