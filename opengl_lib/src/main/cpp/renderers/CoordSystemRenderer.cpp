#include "CoordSystemRenderer.h"
#include "../util/GLUtils.h"
#include <gtc/matrix_transform.hpp>

CoordSystemRenderer::CoordSystemRenderer() {
    m_SamplerLoc = GL_NONE;
    m_MVPMatLoc = GL_NONE;

    m_TextureId = GL_NONE;
    m_VAOId = GL_NONE;

    m_AngleX = 0;
    m_AngleY = 0;
    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;
}

CoordSystemRenderer::~CoordSystemRenderer() {
    NativeImageUtil::FreeNativeImage(&m_RenderImage);
}

void CoordSystemRenderer::Init() {
    if (m_ProgramObj) return;

    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    char vShaderStr[] =
            "#version 300 es                                \n"
            "layout(location = 0) in vec4 a_position;       \n"
            "layout(location = 1) in vec2 a_texCoord;       \n"
            "uniform mat4 u_MVPMatrix;                      \n"
            "out vec2 v_texCoord;                           \n"
            "void main(){                                   \n"
            "   gl_Position = u_MVPMatrix * a_position;     \n"
            "   v_texCoord = a_texCoord;                    \n"
            "}";

    char fShaderStr[] =
            "#version 300 es                                  \n"
            "precision mediump float;                         \n"
            "in vec2 v_texCoord;                              \n"
            "layout(location = 0) out vec4 outColor;          \n"
            "uniform sampler2D s_TextureMap;                  \n"
            "void main(){                                     \n"
            "   outColor = texture(s_TextureMap, v_texCoord); \n"
            "}";

    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
    if (m_ProgramObj) {
        m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
        m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
    } else {
        LOGCATE("CoordSystemRenderer::Init create program fail");
    }
    GLfloat verticesCoords[] = {
            -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
    };

    GLfloat textureCoords[] = {
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f
    };

    GLushort indices[] = {0, 1, 2, 0, 2, 3};

    glGenBuffers(3, m_VBOIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBOIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCoords), verticesCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBOIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords), textureCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VBOIds[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &m_VAOId);
    glBindVertexArray(m_VAOId);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBOIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *) 0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBOIds[1]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (const void *) 0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VBOIds[2]);

    glBindVertexArray(GL_NONE);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width,
                 m_RenderImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
}

void CoordSystemRenderer::LoadImage(NativeImage *pImage) {
    LOGCATE("CoordSystemRenderer::LoadImage pImage = %p", pImage->ppPlane[0]);
    if (pImage) {
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        m_RenderImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
    }
}

void CoordSystemRenderer::Draw(int screenW, int screenH) {
    LOGCATE("CoordSystemRenderer::Draw()");

    if (m_ProgramObj == GL_NONE || m_TextureId == GL_NONE) return;

    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float) screenW / screenH);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width,
                 m_RenderImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    glUseProgram(m_ProgramObj);

    glBindVertexArray(m_VAOId);

    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glUniform1i(m_SamplerLoc, 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *) 0);
}

void CoordSystemRenderer::Destroy() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
        glDeleteBuffers(3, m_VBOIds);
        glDeleteVertexArrays(1, &m_VAOId);
        glDeleteTextures(1, &m_TextureId);
    }
}

void CoordSystemRenderer::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) {
    LOGCATE("CoordSystemRenderer::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX,
            angleY, ratio);
    angleX = angleX % 360;
    angleY = angleY % 360;

    float radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);

    glm::mat4 Projection = glm::perspective(45.0f, ratio, 0.1f, 100.f);

    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, 4),
            glm::vec3(0, 0, 0),
            glm::vec3(0, 1, 0)
    );

    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(m_ScaleX, m_ScaleY, 1.0f));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));

    mvpMatrix = Projection * View * Model;
}

void
CoordSystemRenderer::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLRendererBase::UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
    m_ScaleX = scaleX;
    m_ScaleY = scaleY;
}
