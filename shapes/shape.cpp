#include "shape.h"
#include "OpenGLScene.h"
#include <QFile>
#include <QGLShader>

Shape::Shape(int stacks, int slices)
{
    m_numStacks = stacks;
    m_numSlices = slices;
    m_numVertices = 3;
    m_textured = false;
    m_texId = 0;

    // Initialize the vertex array object.
    glGenVertexArrays(1, &m_vaoID);
    glBindVertexArray(m_vaoID);

    // Initialize the vertex buffer object.
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);

//    initOpenglData();

    // Unbind buffers.
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

Shape::Shape(int stacks, int slices, CS123SceneFileMap* texture) : Shape(stacks, slices) {
    m_textured = texture->isUsed;
    if (m_textured) {
        m_texId = loadTexture(QString::fromStdString(texture->filename));
    }
    m_uScale = texture->repeatU;
    m_vScale = texture->repeatV;

}

GLuint Shape::loadTexture(const QString &path)
{
    QImage texture;
    QFile file(path);
    if(!file.exists()) return -1;
    texture.load(file.fileName());
    texture = QGLWidget::convertToGLFormat(texture);

    // Put your code here
    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexImage2D(GL_TEXTURE_2D,
                 0, //level
                 GL_RGBA, // internal format
                 texture.width(),
                 texture.height(),
                 0, // border... always 0
                 GL_RGBA, // format
                 GL_UNSIGNED_BYTE, // type of data,
                 texture.bits() // pointer to image data
                 );


    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MIN_FILTER,
                    GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,
                    GL_TEXTURE_MAG_FILTER,
                    GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, 0);
    return id; // Return something meaningful
}

Shape::~Shape()
{
    delete[] m_vertexData;
    for (int i = 0; i < m_numVertices / 3; i++) {
        delete m_triangles[i];
    }
    delete[] m_triangles;

}

void Shape::setStacks(int num) {
    m_numStacks = num;
}

void Shape::setSlices(int num) {
    m_numSlices= num;
}

void Shape::render(GLuint shader)
{
//    glActiveTexture(GL_TEXTURE0); // Set the active texture to texture 0.
//    glUniform1i(glGetUniformLocation(shader, "tex"), 0); // Tell the shader to use texture 0.
//    glUniform1i(glGetUniformLocation(shader, "useTexture"), m_textured ? 1 : 0);
    // Draw the shape.
    glBindVertexArray(m_vaoID);
    glDrawArrays(GL_TRIANGLES, 0, m_numVertices);
    glBindVertexArray(0);
}

void Shape::renderNormals(NormalRenderer* nr) {
    // Initialize normals so they can be displayed with arrows. (This can be very helpful
    // for debugging!)
    // This object (m_normalRenderer) can be passed around to other classes,
    // make sure to include "OpenGLScene.h" in any class you want to use the NormalRenderer!
    // generateArrays will take care of any cleanup from the previous object state.
    nr->generateArrays(
                m_vertexData,             // Pointer to vertex data
                6 * sizeof(GLfloat),    // Stride (distance between consecutive vertices/normals in BYTES
                0,                      // Offset of first position in BYTES
                3 * sizeof(GLfloat),    // Offset of first normal in BYTES
                m_numVertices);                     // Number of vertices

}

void Shape::convertTrisToGl(GLuint shader) {
    delete[] m_vertexData;

    if (m_textured) {
        m_vertexData = new GLfloat[8 * m_numVertices]; // data/vertex * numvertices

        for (int i = 0; i < m_numVertices / 3; i++) {
            triangle* tri = m_triangles[i];
            m_vertexData[i*24 + 0] = tri->v1->x;
            m_vertexData[i*24 + 1] = tri->v1->y;
            m_vertexData[i*24 + 2] = tri->v1->z;
            m_vertexData[i*24 + 3] = tri->v1->n->x;
            m_vertexData[i*24 + 4] = tri->v1->n->y;
            m_vertexData[i*24 + 5] = tri->v1->n->z;
            m_vertexData[i*24 + 6] = tri->v1->u;
            m_vertexData[i*24 + 7] = tri->v1->v;

            m_vertexData[i*24 + 8] = tri->v2->x;
            m_vertexData[i*24 + 9] = tri->v2->y;
            m_vertexData[i*24 + 10] = tri->v2->z;
            m_vertexData[i*24 + 11] = tri->v2->n->x;
            m_vertexData[i*24 + 12] = tri->v2->n->y;
            m_vertexData[i*24 + 13] = tri->v2->n->z;
            m_vertexData[i*24 + 14] = tri->v2->u;
            m_vertexData[i*24 + 15] = tri->v2->v;

            m_vertexData[i*24 + 16] = tri->v3->x;
            m_vertexData[i*24 + 17] = tri->v3->y;
            m_vertexData[i*24 + 18] = tri->v3->z;
            m_vertexData[i*24 + 19] = tri->v3->n->x;
            m_vertexData[i*24 + 20] = tri->v3->n->y;
            m_vertexData[i*24 + 21] = tri->v3->n->z;
            m_vertexData[i*24 + 22] = tri->v3->u;
            m_vertexData[i*24 + 23] = tri->v3->v;
        }
       // Pass vertex data to OpenGL.
        glBindVertexArray(m_vaoID);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBindTexture(GL_TEXTURE_2D, m_texId);

        glBufferData(GL_ARRAY_BUFFER, m_numVertices * 8 * sizeof(GLfloat),
                     m_vertexData, GL_STATIC_DRAW);

        glEnableVertexAttribArray(glGetAttribLocation(shader, "position"));
        glEnableVertexAttribArray(glGetAttribLocation(shader, "normal"));
        glEnableVertexAttribArray(glGetAttribLocation(shader, "texCoord"));


        glVertexAttribPointer(
            glGetAttribLocation(shader, "position"),
            3,                   // Num coordinates per position
            GL_FLOAT,            // Type
            GL_FALSE,            // Normalized
            sizeof(GLfloat) * 8, // Stride
            (void*) 0            // Array buffer offset
        );
        glVertexAttribPointer(
            glGetAttribLocation(shader, "normal"),
            3,           // Num coordinates per normal
            GL_FLOAT,    // Type
            GL_TRUE,     // Normalized
            sizeof(GLfloat) * 8,           // Stride
            (void*) (sizeof(GLfloat) * 3)    // Array buffer offset
        );
        glVertexAttribPointer(
            glGetAttribLocation(shader, "texCoord"),
            2,           // Num coordinates per tex coord
            GL_FLOAT,    // Type
            GL_FALSE,     // Normalized
            sizeof(GLfloat) * 8,           // Stride
            (void*) (sizeof(GLfloat) * 6)    // Array buffer offset
        );
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    else {
        m_vertexData = new GLfloat[6 * m_numVertices]; // data/vertex * numvertices

        for (int i = 0; i < m_numVertices / 3; i++) {
            triangle* tri = m_triangles[i];
            m_vertexData[i*18 + 0] = tri->v1->x;
            m_vertexData[i*18 + 1] = tri->v1->y;
            m_vertexData[i*18 + 2] = tri->v1->z;
            m_vertexData[i*18 + 3] = tri->v1->n->x;
            m_vertexData[i*18 + 4] = tri->v1->n->y;
            m_vertexData[i*18 + 5] = tri->v1->n->z;

            m_vertexData[i*18 + 6] = tri->v2->x;
            m_vertexData[i*18 + 7] = tri->v2->y;
            m_vertexData[i*18 + 8] = tri->v2->z;
            m_vertexData[i*18 + 9] = tri->v2->n->x;
            m_vertexData[i*18 + 10] = tri->v2->n->y;
            m_vertexData[i*18 + 11] = tri->v2->n->z;

            m_vertexData[i*18 + 12] = tri->v3->x;
            m_vertexData[i*18 + 13] = tri->v3->y;
            m_vertexData[i*18 + 14] = tri->v3->z;
            m_vertexData[i*18 + 15] = tri->v3->n->x;
            m_vertexData[i*18 + 16] = tri->v3->n->y;
            m_vertexData[i*18 + 17] = tri->v3->n->z;
        }
       // Pass vertex data to OpenGL.
        glBindVertexArray(m_vaoID);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
       glBufferData(GL_ARRAY_BUFFER, m_numVertices * 6 * sizeof(GLfloat),
                    m_vertexData, GL_STATIC_DRAW);
       glEnableVertexAttribArray(glGetAttribLocation(shader, "position"));
       glEnableVertexAttribArray(glGetAttribLocation(shader, "normal"));
       glVertexAttribPointer(
           glGetAttribLocation(shader, "position"),
           3,                   // Num coordinates per position
           GL_FLOAT,            // Type
           GL_FALSE,            // Normalized
           sizeof(GLfloat) * 6, // Stride
           (void*) 0            // Array buffer offset
       );
       glVertexAttribPointer(
           glGetAttribLocation(shader, "normal"),
           3,           // Num coordinates per normal
           GL_FLOAT,    // Type
           GL_TRUE,     // Normalized
           sizeof(GLfloat) * 6,           // Stride
           (void*) (sizeof(GLfloat) * 3)    // Array buffer offset
       );
       glBindBuffer(GL_ARRAY_BUFFER, 0);
       glBindVertexArray(0);
    }
}


TexPair* Shape::getTexVal(double i, double j, double repeat) {
    if (m_textured) {
        double val1 = fmod(i * repeat, 1.0 + m_eps);
        double val2 = fmod(j * repeat, 1.0 + m_eps);
        if (val1 > val2) {
            if (val1 + m_eps >= 1.0) {
                val1 = 0.0;
            } else {
                val2 = 1.0;
            }
        }
        return new TexPair(val1, val2);
    } else {
        return new TexPair(0, 0);
    }
}

GLuint Shape::getTexId() {
    return m_texId;
}
