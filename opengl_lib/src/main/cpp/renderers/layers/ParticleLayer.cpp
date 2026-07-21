#include "ParticleLayer.h"
#include "../../util/GLUtils.h"
#include "../../util/LogUtil.h"
#include "../../util/ShaderLoader.h"
#include "gtc/matrix_transform.hpp"

ParticleLayer::ParticleLayer() 
    : GLLayerBase(LAYER_TYPE_PARTICLE),
      m_ParticleType(0),
      m_ParticleDensity(0.5f),
      m_ParticleColor(0.6f, 0.55f, 0.45f),
      m_ParticleVisibility(0.5f),
      m_ScreenWidth(0),
      m_ScreenHeight(0),
      m_Time(0.0f),
      m_FrameIndex(0),
      m_VaoId(GL_NONE),
      m_MVPMatLoc(GL_NONE),
      m_TimeLoc(GL_NONE),
      m_ScreenSizeLoc(GL_NONE),
      m_ParticleTypeLoc(GL_NONE),
      m_ParticleDensityLoc(GL_NONE),
      m_ParticleColorLoc(GL_NONE),
      m_ParticleVisibilityLoc(GL_NONE) {
    m_VboIds[0] = m_VboIds[1] = m_VboIds[2] = GL_NONE;
}

ParticleLayer::~ParticleLayer() {}

bool ParticleLayer::Init() {
    LOGCATI("ParticleLayer::Init: called, m_ProgramObj=%d", m_ProgramObj);
    if (m_ProgramObj) {
        LOGCATI("ParticleLayer::Init: already initialized, skipping");
        return true;
    }

    std::string vShaderStr = ShaderLoader::LoadShaderFromAssets("shaders/particle_layer_v.glsl");
    std::string fShaderStr = ShaderLoader::LoadShaderFromAssets("shaders/particle_layer_f.glsl");

    if (vShaderStr.empty()) {
        LOGCATE("ParticleLayer::Init: failed to load vertex shader");
        return false;
    }
    if (fShaderStr.empty()) {
        LOGCATE("ParticleLayer::Init: failed to load fragment shader");
        return false;
    }

    m_ProgramObj = GLUtils::CreateProgram(vShaderStr.c_str(), fShaderStr.c_str(), m_VertexShader, m_FragmentShader);
    if (!m_ProgramObj) {
        LOGCATE("ParticleLayer::Init create program fail");
        return false;
    }
    LOGCATI("ParticleLayer::Init success, m_ProgramObj=%d", m_ProgramObj);

    m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
    m_TimeLoc = glGetUniformLocation(m_ProgramObj, "u_time");
    m_ScreenSizeLoc = glGetUniformLocation(m_ProgramObj, "u_screenSize");
    m_ParticleTypeLoc = glGetUniformLocation(m_ProgramObj, "u_particleType");
    m_ParticleDensityLoc = glGetUniformLocation(m_ProgramObj, "u_particleDensity");
    m_ParticleColorLoc = glGetUniformLocation(m_ProgramObj, "u_particleColor");
    m_ParticleVisibilityLoc = glGetUniformLocation(m_ProgramObj, "u_particleVisibility");

    // 顶点坐标
    GLfloat verticesCoords[] = {
            -1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 0.0f,
            1.0f, -1.0f, 0.0f,
            1.0f, 1.0f, 0.0f
    };

    // 纹理坐标
    GLfloat textureCoords[] = {
            0.0f, 0.0f,
            0.0f, 1.0f,
            1.0f, 1.0f,
            1.0f, 0.0f
    };

    // 索引
    GLushort indices[] = {0, 1, 2, 0, 2, 3};

    glGenBuffers(3, m_VboIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCoords), verticesCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords), textureCoords, GL_STATIC_DRAW);

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
    return true;
}

void ParticleLayer::Draw(int screenW, int screenH) {
    if (!m_ProgramObj) {
        LOGCATE("ParticleLayer::Draw: m_ProgramObj is null, skipping");
        return;
    }
    if (!m_Enabled) {
        LOGCATE("ParticleLayer::Draw: layer disabled, skipping");
        return;
    }

    m_ScreenWidth = screenW;
    m_ScreenHeight = screenH;

    UpdateMVPMatrix();

    glUseProgram(m_ProgramObj);

    glBindVertexArray(m_VaoId);

    // 更新时间
    m_FrameIndex++;
    m_Time = m_FrameIndex * 0.04f;

    // 设置 uniform 参数
    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    glUniform1f(m_TimeLoc, m_Time);
    glUniform2f(m_ScreenSizeLoc, screenW, screenH);
    glUniform1i(m_ParticleTypeLoc, m_ParticleType);
    glUniform1f(m_ParticleDensityLoc, m_ParticleDensity);
    glUniform3f(m_ParticleColorLoc, m_ParticleColor.x, m_ParticleColor.y, m_ParticleColor.z);
    glUniform1f(m_ParticleVisibilityLoc, m_ParticleVisibility);

    // 启用混合模式，实现半透明颗粒效果
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *) 0);

    glDisable(GL_BLEND);

    glBindVertexArray(GL_NONE);
}

void ParticleLayer::Destroy() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
        glDeleteBuffers(3, m_VboIds);
        glDeleteVertexArrays(1, &m_VaoId);
        m_ProgramObj = GL_NONE;
    }
}

void ParticleLayer::SetParamInt(LayerParamType paramType, int value) {
    switch (paramType) {
        case PARAM_PARTICLE_TYPE:
            m_ParticleType = value;
            break;
        default:
            break;
    }
}

void ParticleLayer::SetParamFloat(LayerParamType paramType, float value) {
    switch (paramType) {
        case PARAM_PARTICLE_DENSITY:
            m_ParticleDensity = value;
            break;
        case PARAM_PARTICLE_COLOR_R:
            m_ParticleColor.x = value;
            break;
        case PARAM_PARTICLE_COLOR_G:
            m_ParticleColor.y = value;
            break;
        case PARAM_PARTICLE_COLOR_B:
            m_ParticleColor.z = value;
            break;
        case PARAM_PARTICLE_VISIBILITY:
            m_ParticleVisibility = value;
            break;
        default:
            break;
    }
}

void ParticleLayer::SetParamVec3(LayerParamType paramType, float x, float y, float z) {
    switch (paramType) {
        case PARAM_SKY_COLOR_TOP:
        case PARAM_SKY_COLOR_BOTTOM:
            m_ParticleColor = glm::vec3(x, y, z);
            break;
        default:
            break;
    }
}

void ParticleLayer::UpdateMVPMatrix() {
    float ratio = (float) m_ScreenWidth / m_ScreenHeight;

    glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    glm::mat4 View = glm::lookAt(glm::vec3(0, 0, 4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 Model = glm::mat4(1.0f);

    m_MVPMatrix = Projection * View * Model;
}