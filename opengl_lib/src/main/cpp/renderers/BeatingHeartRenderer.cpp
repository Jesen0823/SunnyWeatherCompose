#include <gtc/matrix_transform.hpp>
#include "BeatingHeartRenderer.h"
#include "../util/GLUtils.h"

BeatingHeartRenderer::BeatingHeartRenderer() {
    m_SamplerLoc = GL_NONE;
    m_MVPMatLoc = GL_NONE;

    m_TextureId = GL_NONE;
    m_VaoId = GL_NONE;

    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;
}

BeatingHeartRenderer::~BeatingHeartRenderer() {
    NativeImageUtil::FreeNativeImage(&m_RenderImage);
}

void BeatingHeartRenderer::Init() {
    if (m_ProgramObj)
        return;

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
            "uniform mat4 u_MVPMatrix;                      \n"
            "void main(){                                   \n"
            "    gl_Position = u_MVPMatrix * a_position;    \n"
            "}";

    char fShaderStr[] =
            "#version 300 es                                                                 \n"
            "precision highp float;                                                          \n"
            "layout(location = 0) out vec4 outColor;                                         \n"
            "uniform float u_time;                                                           \n"
            "uniform vec2 u_screenSize;                                                      \n"
            "void main(){                                                                    \n"
            "    vec2 fragCoord = gl_FragCoord.xy;                                           \n"
            "    vec2 p = (2.0*fragCoord-u_screenSize.xy)/min(u_screenSize.y,u_screenSize.x);\n"
            "    vec2 uv = fragCoord / u_screenSize;                                         \n"
            "    vec3 bcol = vec3(1.0,0.8,0.8)*(1.0-0.38*length(p));                         \n"
            "    float tt = u_time;                                                          \n"
            "    float ss = pow(tt,.2)*0.5 + 0.5;                                            \n"
            "    ss = 1.0 + ss*0.5*sin(tt*6.2831*3.0 + p.y*0.5)*exp(-tt*4.0);                \n"
            "    p *= vec2(0.5,1.5) + ss*vec2(0.5,-0.5);                                     \n"
            "    p.y -= 0.25;                                                                \n"
            "    float a = atan(p.x,p.y)/3.141592653;                                        \n"
            "    float r = length(p);                                                        \n"
            "    float h = abs(a);                                                           \n"
            "    float d = (13.0*h - 22.0*h*h + 10.0*h*h*h)/(6.0-5.0*h);                     \n"
            "    float s = 0.75 + 0.75*p.x;                                                  \n"
            "    s *= 1.0-0.4*r;                                                             \n"
            "    s = 0.3 + 0.7*s;                                                            \n"
            "    s *= 0.5+0.5*pow( 1.0-clamp(r/d, 0.0, 1.0 ), 0.1 );                         \n"
            "    vec3 hcol = vec3(1.0,0.5*r,0.3)*s;                                          \n"
            "    vec3 col = mix( bcol, hcol, smoothstep( -0.06, 0.06, d-r) );                \n"
            "    outColor = vec4(col,1.0);                                                   \n"
            "}";

    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
    if (m_ProgramObj) {
        m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
        m_SizeLoc = glGetUniformLocation(m_ProgramObj, "u_screenSize");
        m_TimeLoc = glGetUniformLocation(m_ProgramObj, "u_time");
    } else {
        LOGCATE("BeatingHeartRenderer::Init create program fail");
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

    glGenBuffers(3, m_VboIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCoords), verticesCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &m_VaoId);
    glBindVertexArray(m_VaoId);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (const void *) 0);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[2]);

    glBindVertexArray(GL_NONE);
}

void BeatingHeartRenderer::LoadImage(NativeImage *pImage) {
    LOGCATE("BeatingHeartRenderer::LoadImage pImage = %p", pImage->ppPlane[0]);
    if (pImage) {
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        m_RenderImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
    }
}

void BeatingHeartRenderer::Draw(int screenW, int screenH) {
    LOGCATE("BeatingHeartRenderer::Draw()");

    if (m_ProgramObj == GL_NONE) return;

    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float) screenW / screenH);

    glUseProgram(m_ProgramObj);

    glBindVertexArray(m_VaoId);

    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    float time = static_cast<float>(fmod(GetSysCurrentTime(), 2000) / 2000);
    LOGCATE("BeatingHeartRenderer::Draw() time=%f", time);
    glUniform1f(m_TimeLoc, time);
    glUniform2f(m_SizeLoc, screenW, screenH);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *) 0);
}

void BeatingHeartRenderer::Destroy() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
        glDeleteBuffers(3, m_VboIds);
        glDeleteVertexArrays(1, &m_VaoId);
        glDeleteTextures(1, &m_TextureId);
    }
}

void BeatingHeartRenderer::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX,
                                               float scaleY) {
    GLRendererBase::UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
    m_ScaleX = scaleX;
    m_ScaleY = scaleY;
}

void
BeatingHeartRenderer::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) {
    LOGCATE("BeatingHeartRenderer::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX,
            angleY, ratio);
    angleX = angleX % 360;
    angleY = angleY % 360;

    float radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    float radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);

    glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);

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
