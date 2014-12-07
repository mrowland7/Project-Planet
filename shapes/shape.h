#ifndef SHAPE_H
#define SHAPE_H
//#include "OpenGLScene.h"
#include "CS123SceneData.h"
#define PI 3.14159265

class NormalRenderer;

struct normal {
    GLfloat x;
    GLfloat y;
    GLfloat z;

    normal() : x(0), y(0), z(0) {}
    normal(GLfloat a, GLfloat b, GLfloat c)
        :x(a), y(b), z(c) {
        glm::vec3 v = glm::normalize(glm::vec3(x, y, z));
        x = v.x;
        y = v.y;
        z = v.z;
    }
};

struct vertex {
    GLfloat x;
    GLfloat y;
    GLfloat z;
    normal* n;
    // Texture coords. Defaults to 0
    GLfloat u;
    GLfloat v;

    vertex() : x(0), y(0), z(0), n(NULL) {}
    vertex(GLfloat a, GLfloat b, GLfloat c, normal* no)
        :x(a), y(b), z(c), n(no), u(0), v(0) {}
    vertex(GLfloat a, GLfloat b, GLfloat c, normal* no, GLfloat d, GLfloat e)
        :x(a), y(b), z(c), n(no), u(d), v(e){}
    ~vertex() {
        delete n;
    }
};

struct triangle {
    vertex* v1;
    vertex* v2;
    vertex* v3;

    triangle(): v1(NULL), v2(NULL), v3(NULL) {}
    triangle(vertex* ve1, vertex* ve2, vertex* ve3)
        : v1(ve1), v2(ve2), v3(ve3) {}
    ~triangle() {
        delete v1;
        delete v2;
        delete v3;
    }
};

struct TexPair {
    double a;
    double b;

    TexPair(double aa, double bb) :
        a(aa), b(bb) {}

};
/**
 * @class Shape
 *
 * The superclass for various types of shapes.
 */
class Shape
{
public:
    Shape(int stacks, int slices);
    Shape(int stacks, int slices, CS123SceneFileMap* texture);
    virtual ~Shape();

    // Set tesselations parameters (p1 and p2 respectively).
    // Allow for overriding by subclasses to keep minimum of 3 if needed
    virtual void setStacks(int num);
    virtual void setSlices(int num);
    GLuint loadTexture(const QString &path);

    void render(GLuint shader);
    virtual void initOpenglData(GLuint shader) = 0;
    virtual void renderNormals(NormalRenderer* renderer);
    virtual void convertTrisToGl(GLuint shader);
    TexPair* getTexVal(double i, double j, double repeat);
    GLuint getTexId();

protected:

    int m_numStacks;
    int m_numSlices;
    int m_numVertices;

    triangle** m_triangles;
    GLfloat* m_vertexData;

    GLuint m_vaoID;
    GLuint m_vbo;

    bool m_textured;
    float m_uScale;
    float m_vScale;
    GLuint m_texId;

    double m_eps = 0.000001;

};


struct ShapeMaterial {
    Shape* shape;
    CS123SceneMaterial* material;
    glm::mat4x4 mat;

    ShapeMaterial(Shape* s, CS123SceneMaterial* m, glm::mat4x4 ma) :
        shape(s), material(m), mat(ma) {}

    ~ShapeMaterial() {
        delete shape;
        delete material;
    }
};

#endif // SHAPE_H
