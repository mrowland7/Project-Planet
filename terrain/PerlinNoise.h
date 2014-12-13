#ifndef PERLINNOISE_H
#define PERLINNOISE_H

#include "GL/glew.h"
#include <glm/glm.hpp>


class PerlinNoise
{
public:
    PerlinNoise();
    ~PerlinNoise();
    glm::vec2 *m_grid;
    int m_gridWidth;
    void noise(float *outGrid, int width, int numOctaves, float persistence, float scale, float freq);


private:
    int getIndex(int x, int y);
    float lerp(float x1, float x2, float c);
    float getNoise(float x, float y);
    void makeOctave(float *outGrid, int width, float frequency, float amplitude);
};

#endif // PERLINNOISE_H
