#ifndef OPENGLSCENE_H
#define OPENGLSCENE_H

#include "Scene.h"
#include <map>

// Maximum number of lights, as defined in shader.
#define MAX_NUM_LIGHTS 10

class SupportCanvas3D;

using std::string;

/** Holds onto the data necessary to render the normals of a mesh
   This struct can be resused for new meshes by calling generateArrays again */
struct NormalRenderer
{
    GLuint vbo, lineVao, triVao, shader;
    GLint offsetLoc;
    GLsizei count;

    /** Pass the shader that will render the normals in the constructor */
    NormalRenderer(GLuint shader) : shader(shader)
    {
        offsetLoc = glGetUniformLocation(shader, "useArrowOffsets");
        vbo = lineVao = triVao = 0;
    }

    /**
     * @param vertexData A pointer to the float array containing the vertex data
     * @param stride The distance in bytes between consecutive positions and normals. This currently only
     * supports cases where the value is equal for normals and positions, but it wouldn't be difficult
     * to make it support different strides.
     * @param positionOffset The offset in bytes of the first position in vertexData
     * @param normalOffset The offset in bytes of the first normal in vertexData
     * @param count The number of vertices
     *
     * This function will output errors if it could not, given stride and the offsets, find count vertices.
     */
    void generateArrays(const GLfloat *vertexData, GLuint stride, GLuint positionOffset, GLuint normalOffset, GLsizei count)
    {
        clearArrays();

        this->count = count;
        if (positionOffset > stride * count || normalOffset > stride * count)
        {
            std::cerr << "Normal Renderer: positionOffset or normalOffset is out of bounds" << std::endl;
            return;
        }
        unsigned int size = count * 6 /* Line Data */ + count * 18 /* Triangle Data */ + count * 3 /* Arrow Offset Data */;
        GLfloat *data = new GLfloat[size];

        // (char *) so we can add byte values
        const char *currentVertex = (char *) vertexData, *currentNormal = (char * )vertexData;

        // Pointers for conveniently accessing data
        GLfloat* lineData = data;
        GLfloat* triData = data + 6 * count;
        GLfloat* offsetData = triData + count * 18;

        int i = 0;
        char * maxData = ((char *) vertexData) + stride * count; // Don't go past this point (try to prevent crashes from bad paremeters)
        for (currentVertex += positionOffset, currentNormal += normalOffset;
             currentVertex < maxData && currentNormal < maxData && i < count;
             currentVertex += stride, currentNormal += stride, i++)
        {
            for (int j = 0; j < 3; j++) // Go through each element of the vertex/normal (i.e. x, y, z)
            {
                GLfloat vertexVal = ((GLfloat *)currentVertex)[j];
                GLfloat normalVal = ((GLfloat *)currentNormal)[j];

                lineData[0] = vertexVal;
                lineData[3] = vertexVal + 0.1f * normalVal;
                lineData++;

                // Interleave positions and normals for the triangles
                triData[0] = vertexVal + 0.15f * normalVal;
                triData[6] = triData[12] = vertexVal + 0.1f * normalVal;
                triData[3] = triData[9] = triData[15] = normalVal;
                triData++;
            }
            lineData += 3;
            triData += 15;

            offsetData[0] = 0.f;
            offsetData[1] = -0.015f;
            offsetData[2] = 0.015f;

            offsetData += 3;
        }
        if (i != count)
        {
            std::cerr << "Normal Renderer: could not find count vertices" << std::endl;
            delete[] data;
            return;
        }

        glGenVertexArrays(1, &lineVao);
        glGenVertexArrays(1, &triVao);

        glGenBuffers(1, &vbo);

        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * size, data, GL_STATIC_DRAW);

        GLint position = glGetAttribLocation(shader, "position");
        GLint normal = glGetAttribLocation(shader, "normal");
        GLint offset = glGetAttribLocation(shader, "arrowOffset");


        glBindVertexArray(lineVao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 0, (void *)0);
        glEnableVertexAttribArray(position);

        glBindVertexArray(triVao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, sizeof(GLfloat) * 6, (void *)(sizeof(GLfloat) * 6 * count));
        glVertexAttribPointer(normal, 3, GL_FLOAT, GL_TRUE, sizeof(GLfloat) * 6, (void *)(sizeof(GLfloat) * (6 * count + 3)));
        glVertexAttribPointer(offset, 1, GL_FLOAT, GL_FALSE, 0, (void *)(sizeof(GLfloat) * (6 * count + 18 * count)));
        glEnableVertexAttribArray(position);
        glEnableVertexAttribArray(normal);
        glEnableVertexAttribArray(offset);

        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        delete[] data;
    }

    /** draw the normals */
    void draw()
    {
        if (lineVao)
        {
            glBindVertexArray(lineVao);
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            glDrawArrays(GL_LINES, 0, count * 2);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }

        if (triVao)
        {
            glBindVertexArray(triVao);
            glUniform1i(offsetLoc, GL_TRUE);
            glDrawArrays(GL_TRIANGLES, 0, count * 3);
        }

        glBindVertexArray(0);
    }

    /** Clean up and clear the vaos/vbo */
    void clearArrays()
    {
        glDeleteBuffers(1, &vbo);
        glDeleteVertexArrays(1, &lineVao);
        glDeleteVertexArrays(1, &triVao);

        vbo = lineVao = triVao = 0;
    }

    ~NormalRenderer()
    {
        clearArrays();
    }
};

/**
 * @class  OpenGLScene
 *
 * Basic Scene implementation that supports OpenGL. Students will extend this class in ShapesScene
 * and SceneviewScene.
 */
class OpenGLScene : public Scene
{
public:
    OpenGLScene();
    virtual ~OpenGLScene();

    // Override this to do any initialization for the scene.
    virtual void init();

    // Render the scene.
    void render(SupportCanvas3D *context);

    // Renders arrows for the normals that were initialized using initNormals(...).
    void renderNormals();

    //stuff to do on settings changed
    virtual void settingsChanged();
    std::vector<ShapeInfo*> getShapes();

protected:
    // Set all lights to black.
    void clearLights();

    // Set the light uniforms for the lights in the scene. (View matrix is used in cases where a
    // light follows the camera, as in ShapesScene.)
    virtual void setLights(const glm::mat4 viewMatrix) = 0;

    // Render geometry for Shapes and Sceneview.
    virtual void renderGeometry() = 0;

    // Set the necessary uniforms to switch materials.
    void applyMaterial(const CS123SceneMaterial &material);

    // Set the necessary uniforms for the light properties.
    void setLight(const CS123SceneLightData &light);

    // The program ID for OpenGL.
    GLuint m_shader;

    // The normal renderer, which can be used to render arrows for each normal in Shapes.
    NormalRenderer *m_normalRenderer;

private:

    GLuint m_normalLinesVaoID, m_normalTrisVaoID; // The VAO IDs for rendering normals.
    GLint m_numNormals; // The number of normals initialized using initializeNormals(...).
    std::map<string, GLint> m_uniformLocs;
};

#endif // OPENGLSCENE_H
