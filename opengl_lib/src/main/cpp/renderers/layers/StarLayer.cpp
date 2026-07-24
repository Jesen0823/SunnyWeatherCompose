#include "StarLayer.h"
#include "../../util/GLUtils.h"
#include "../../util/LogUtil.h"
#include "../../util/ShaderLoader.h"

static const char *TAG = "StarLayer";

struct StarVertex {
    glm::vec2 pos;
    glm::vec3 color;
    float size;
    float twinkleOffset;
};

StarLayer::StarLayer() : GLLayerBase(LAYER_TYPE_STAR) {
    GenerateStars();
}

StarLayer::~StarLayer() {
    Destroy();
}

void StarLayer::GenerateStars() {
    m_StarPositions.resize(m_StarCount);
    m_StarColors.resize(m_StarCount);
    m_StarSizes.resize(m_StarCount);
    m_StarTwinkleOffsets.resize(m_StarCount);

    const int clusterCount = 6;
    std::vector<glm::vec2> clusters(clusterCount);
    for (int i = 0; i < clusterCount; i++) {
        clusters[i] = glm::vec2((float)rand() / RAND_MAX, (float)rand() / RAND_MAX);
    }

    for (int i = 0; i < m_StarCount; i++) {
        glm::vec2 pos;
        float clusterProb = (float)rand() / RAND_MAX;
        
        if (clusterProb < 0.6f) {
            int clusterIdx = rand() % clusterCount;
            float sigma = 0.06f + ((float)rand() / RAND_MAX) * 0.06f;
            float angle = ((float)rand() / RAND_MAX) * 6.28f;
            float dist = sqrt(-2.0f * log(1.0f - (float)rand() / RAND_MAX)) * sigma;
            pos = clusters[clusterIdx] + glm::vec2(cos(angle) * dist, sin(angle) * dist);
        } else {
            pos = glm::vec2((float)rand() / RAND_MAX, (float)rand() / RAND_MAX);
        }

        pos.x = fmax(0.0f, fmin(1.0f, pos.x));
        pos.y = fmax(0.0f, fmin(1.0f, pos.y));
        m_StarPositions[i] = pos;

        float sizeRand = (float)rand() / RAND_MAX;
        float size, brightness;
        
        if (sizeRand < 0.1f) {
            size = 16.0f + ((float)rand() / RAND_MAX) * 10.0f;
            brightness = 0.9f + ((float)rand() / RAND_MAX) * 0.1f;
        } else if (sizeRand < 0.4f) {
            size = 10.0f + ((float)rand() / RAND_MAX) * 6.0f;
            brightness = 0.75f + ((float)rand() / RAND_MAX) * 0.25f;
        } else {
            size = 6.0f + ((float)rand() / RAND_MAX) * 4.0f;
            brightness = 0.5f + ((float)rand() / RAND_MAX) * 0.3f;
        }
        m_StarSizes[i] = size;

        float colorVariation = ((float)rand() / RAND_MAX);
        if (colorVariation < 0.65f) {
            m_StarColors[i] = glm::vec3(brightness, brightness, brightness);
        } else if (colorVariation < 0.82f) {
            m_StarColors[i] = glm::vec3(brightness * 0.88f, brightness * 0.91f, brightness);
        } else {
            m_StarColors[i] = glm::vec3(brightness, brightness * 0.88f, brightness * 0.78f);
        }

        m_StarTwinkleOffsets[i] = ((float)rand() / RAND_MAX) * 6.28f;
    }
}

bool StarLayer::Init() {
    if (m_ProgramObj != 0) {
        LOGCATI("StarLayer::Init: already initialized, m_ProgramObj=%d", m_ProgramObj);
        return true;
    }

    LOGCATI("StarLayer::Init: called, m_ProgramObj=%d, m_StarCount=%d", m_ProgramObj, m_StarCount);

    std::string vShaderStr = ShaderLoader::LoadShaderFromAssets("shaders/star_layer_v.glsl");
    std::string fShaderStr = ShaderLoader::LoadShaderFromAssets("shaders/star_layer_f.glsl");

    if (vShaderStr.empty()) {
        LOGCATE("StarLayer::Init: failed to load vertex shader");
        return false;
    }
    if (fShaderStr.empty()) {
        LOGCATE("StarLayer::Init: failed to load fragment shader");
        return false;
    }

    m_ProgramObj = GLUtils::CreateProgram(vShaderStr.c_str(), fShaderStr.c_str(), m_VertexShader, m_FragmentShader);
    if (m_ProgramObj == 0) {
        LOGCATI("StarLayer::Init create program fail");
        return false;
    }

    m_PositionLoc = glGetAttribLocation(m_ProgramObj, "a_position");
    m_ColorLoc = glGetAttribLocation(m_ProgramObj, "a_color");
    m_SizeLoc = glGetAttribLocation(m_ProgramObj, "a_size");
    m_TwinkleOffsetLoc = glGetAttribLocation(m_ProgramObj, "a_twinkleOffset");
    m_TimeLoc = glGetUniformLocation(m_ProgramObj, "u_time");
    m_ScreenSizeLoc = glGetUniformLocation(m_ProgramObj, "u_screenSize");

    LOGCATI("StarLayer::Init: attrib locations - pos=%d, color=%d, size=%d, twinkle=%d", 
            m_PositionLoc, m_ColorLoc, m_SizeLoc, m_TwinkleOffsetLoc);
    LOGCATI("StarLayer::Init: uniform locations - time=%d, screenSize=%d", 
            m_TimeLoc, m_ScreenSizeLoc);

    std::vector<StarVertex> vertices(m_StarCount);
    for (int i = 0; i < m_StarCount; i++) {
        vertices[i].pos = m_StarPositions[i];
        vertices[i].color = m_StarColors[i];
        vertices[i].size = m_StarSizes[i];
        vertices[i].twinkleOffset = m_StarTwinkleOffsets[i];
    }

    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, m_StarCount * sizeof(StarVertex), vertices.data(), GL_STATIC_DRAW);

    const size_t stride = sizeof(StarVertex);
    glVertexAttribPointer(m_PositionLoc, 2, GL_FLOAT, GL_FALSE, stride, (const void*)offsetof(StarVertex, pos));
    glEnableVertexAttribArray(m_PositionLoc);

    glVertexAttribPointer(m_ColorLoc, 3, GL_FLOAT, GL_FALSE, stride, (const void*)offsetof(StarVertex, color));
    glEnableVertexAttribArray(m_ColorLoc);

    glVertexAttribPointer(m_SizeLoc, 1, GL_FLOAT, GL_FALSE, stride, (const void*)offsetof(StarVertex, size));
    glEnableVertexAttribArray(m_SizeLoc);

    glVertexAttribPointer(m_TwinkleOffsetLoc, 1, GL_FLOAT, GL_FALSE, stride, (const void*)offsetof(StarVertex, twinkleOffset));
    glEnableVertexAttribArray(m_TwinkleOffsetLoc);

    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    m_VBO = vbo;
    m_VaoId = vao;

    m_StarPositions.clear();
    m_StarColors.clear();
    m_StarSizes.clear();
    m_StarTwinkleOffsets.clear();

    LOGCATI("StarLayer::Init success, m_ProgramObj=%d, starCount=%d", m_ProgramObj, m_StarCount);
    return true;
}

void StarLayer::Draw(int screenW, int screenH) {
    if (m_ProgramObj == 0 || !m_Enabled) {
        return;
    }

    m_Time += 0.016f * m_TwinkleSpeed;

    glUseProgram(m_ProgramObj);

    glUniform1f(m_TimeLoc, m_Time);
    glUniform2f(m_ScreenSizeLoc, (float)screenW, (float)screenH);

    glBindVertexArray(m_VaoId);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glDrawArrays(GL_POINTS, 0, m_StarCount);

    glBindVertexArray(0);
    glDisable(GL_BLEND);
    glDepthMask(GL_TRUE);
}

void StarLayer::Destroy() {
    if (m_ProgramObj != 0) {
        glDeleteProgram(m_ProgramObj);
        m_ProgramObj = 0;
    }
    if (m_VBO != 0) {
        glDeleteBuffers(1, &m_VBO);
        m_VBO = 0;
    }
    if (m_VaoId != 0) {
        glDeleteVertexArrays(1, &m_VaoId);
        m_VaoId = 0;
    }
}

void StarLayer::SetParamFloat(LayerParamType paramType, float value) {
    switch (paramType) {
        case PARAM_STAR_COUNT:
            m_StarCount = static_cast<int>(value);
            GenerateStars();
            break;
        case PARAM_STAR_TWINKLE_SPEED:
            m_TwinkleSpeed = value;
            break;
        default:
            break;
    }
}

void StarLayer::SetParamVec3(LayerParamType paramType, float x, float y, float z) {
    // StarLayer 不需要 vec3 参数
}