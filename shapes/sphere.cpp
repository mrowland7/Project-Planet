#include "sphere.h"
#include "shape.h"
#include "math.h"

Sphere::Sphere(int stacks, int slices)
    : Shape(stacks, slices)
{
    m_numVertices = 5;
    m_vertexData = NULL;
    m_radius = .5;
    m_numSlices = MAX(slices, 3);
    m_numStacks = MAX(stacks, 2);
}
Sphere::Sphere(int stacks, int slices, CS123SceneFileMap* tex)
    : Shape(stacks, slices, tex)
{
    m_numVertices = 5;
    m_vertexData = NULL;
    m_radius = .5;
    m_numSlices = MAX(slices, 3);
    m_numStacks = MAX(stacks, 2);
}

Sphere::~Sphere()
{

}

void Sphere::setSlices(int num) {
    m_numSlices = MAX(num, 3);
}

void Sphere::setStacks(int num) {
    m_numStacks= MAX(num, 2);
}

void Sphere::initOpenglData(GLuint shader)
{
    double latStart = 0;
    double latStep = PI / m_numStacks;
    double lngStart = 0;
    double lngStep = 2 * PI / m_numSlices;
    double lat, lng;
    int numTris = m_numStacks * m_numSlices * 2;
    m_triangles = new triangle*[numTris];
    m_numVertices = numTris * 3;

    for (double i = 0; i < m_numStacks; i++) {
        for (double j = 0; j < m_numSlices; j++) {
            lat = latStart + i*latStep;
            lng = lngStart + j*lngStep;
            int bufIndex = i * m_numSlices * 2 + j * 2;
            addTriangles(lat, lng, bufIndex);

        }
    }

    convertTrisToGl(shader);
}


void Sphere::addTriangles(double lat, double lng, int bufIndex)
{
    double latStep = PI / m_numStacks;
    double lngStep = 2 * PI / m_numSlices;

    double x1 = m_radius * sin(lat) * cos(lng);
    double y1 = m_radius * cos(lat);
    double z1 = m_radius * sin(lat) * sin(lng);

    double x2 = m_radius * sin(lat) * cos(lng + lngStep);
    double y2 = m_radius * cos(lat);
    double z2 = m_radius * sin(lat) * sin(lng + lngStep);

    double x3 = m_radius * sin(lat + latStep) * cos(lng);
    double y3 = m_radius * cos(lat + latStep);
    double z3 = m_radius * sin(lat + latStep) * sin(lng);

    double x4 = m_radius * sin(lat + latStep) * cos(lng + lngStep);
    double y4 = m_radius * cos(lat + latStep);
    double z4 = m_radius * sin(lat + latStep) * sin(lng + lngStep);


    TexPair* u = getTexVal(lng/(2*M_PI), (lng+lngStep)/(2*M_PI), m_uScale);
    double u1 = u->a;
    double u2 = u->b;
    delete u;
    TexPair* v = getTexVal(lat/(2*M_PI), (lat+latStep)/(2*M_PI), m_vScale);
    double v1 = v->a;
    double v2 = v->b;
    delete v;

    normal* n1 = new normal(x1 / m_radius, y1 / m_radius, z1 / m_radius);
    normal* n2a = new normal(x2 / m_radius, y2 / m_radius, z2 / m_radius);
    normal* n2b = new normal(x2 / m_radius, y2 / m_radius, z2 / m_radius);
    normal* n3a = new normal(x3 / m_radius, y3 / m_radius, z3 / m_radius);
    normal* n3b = new normal(x3 / m_radius, y3 / m_radius, z3 / m_radius);
    normal* n4 = new normal(x4 / m_radius, y4 / m_radius, z4 / m_radius);
    vertex* ve1 = new vertex(x1, y1, z1, n1, u1, v1);
    vertex* ve2a = new vertex(x2, y2, z2, n2a, u2, v1);
    vertex* ve2b = new vertex(x2, y2, z2, n2b, u2, v1);
    vertex* ve3a = new vertex(x3, y3, z3, n3a, u1, v2);
    vertex* ve3b = new vertex(x3, y3, z3, n3b, u1, v2);
    vertex* ve4 = new vertex(x4, y4, z4, n4, u2, v2);

    m_triangles[bufIndex] = new triangle(ve1, ve2a, ve3a);
    m_triangles[bufIndex + 1] = new triangle(ve3b, ve2b, ve4);
}
