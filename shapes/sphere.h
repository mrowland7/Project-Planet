#ifndef SPHERE_H
#define SPHERE_H
#include "shape.h"

class Sphere : public Shape
{
public:
    Sphere(int stacks, int slices);
    Sphere(int stacks, int slices, CS123SceneFileMap* tex);
    virtual ~Sphere();

    virtual void setSlices(int num);
    virtual void setStacks(int num);
    virtual void initOpenglData(GLuint shader);
    void addTriangles(double lat, double lng, int bufIndex);

protected:
    double m_radius;
};

#endif // SPHERE_H
