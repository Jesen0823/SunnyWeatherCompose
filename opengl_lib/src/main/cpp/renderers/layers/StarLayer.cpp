#include "StarLayer.h"
#include "../../util/GLUtils.h"
#include "../../util/LogUtil.h"

static const char *TAG = "StarLayer";

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
            size = 8.0f + ((float)rand() / RAND_MAX) * 5.0f;
            brightness = 0.9f + ((float)rand() / RAND_MAX) * 0.1f;
        } else if (sizeRand < 0.4f) {
            size = 5.0f + ((float)rand() / RAND_MAX) * 3.0f;
            brightness = 0.75f + ((float)rand() / RAND_MAX) * 0.25f;
        } else {
            size = 3.0f + ((float)rand() / RAND_MAX) * 2.0f;
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

    const char *vShaderStr =
        "#version 300 es                          \n"
        "precision highp float;                   \n"
        "layout(location = 0) in vec2 a_position; \n"
        "layout(location = 1) in vec3 a_color;    \n"
        "layout(location = 2) in float a_size;    \n"
        "layout(location = 3) in float a_twinkleOffset;\n"
        "out vec3 v_color;                        \n"
        "out float v_twinkleOffset;               \n"
        "uniform vec2 u_screenSize;               \n"
        "void main() {                            \n"
        "    vec2 clipPos = a_position * 2.0 - 1.0;\n"
        "    clipPos.x *= u_screenSize.x / u_screenSize.y;\n"
        "    gl_Position = vec4(clipPos, 0.0, 1.0);\n"
        "    gl_PointSize = a_size;               \n"
        "    v_color = a_color;                   \n"
        "    v_twinkleOffset = a_twinkleOffset;   \n"
        "}";

    const char *fShaderStr =
        "#version 300 es                          \n"
        "precision highp float;                   \n"
        "in vec3 v_color;                         \n"
        "in float v_twinkleOffset;                \n"
        "out vec4 outColor;                       \n"
        "uniform float u_time;                    \n"
        "void main() {                            \n"
        "    vec2 coord = gl_PointCoord - vec2(0.5);\n"
        "    float dist = length(coord);          \n"
        "    if (dist > 0.5) discard;             \n"
        "    float twinkle = sin(u_time * 2.0 + v_twinkleOffset) * 0.3 + sin(u_time * 5.0 + v_twinkleOffset * 1.7) * 0.2 + 0.5;\n"
        "    float core = 1.0 - smoothstep(0.0, 0.25, dist);\n"
        "    float glow = 1.0 - smoothstep(0.0, 0.5, dist);\n"
        "    float spikeX = exp(-abs(coord.x) * 12.0) * (1.0 - smoothstep(0.0, 0.45, abs(coord.y)));\n"
        "    float spikeY = exp(-abs(coord.y) * 12.0) * (1.0 - smoothstep(0.0, 0.45, abs(coord.x)));\n"
        "    float spikes = (spikeX + spikeY) * 0.35;\n"
        "    float finalAlpha = core * twinkle + glow * twinkle * 0.4 + spikes * twinkle;\n"
        "    outColor = vec4(v_color * twinkle, finalAlpha);\n"
        "}";

    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
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

    glGenBuffers(1, &m_VBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, m_StarCount * sizeof(glm::vec2), m_StarPositions.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_ColorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_ColorVBO);
    glBufferData(GL_ARRAY_BUFFER, m_StarCount * sizeof(glm::vec3), m_StarColors.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_SizeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_SizeVBO);
    glBufferData(GL_ARRAY_BUFFER, m_StarCount * sizeof(float), m_StarSizes.data(), GL_STATIC_DRAW);

    glGenBuffers(1, &m_TwinkleVBO);
    glBindBuffer(GL_ARRAY_BUFFER, m_TwinkleVBO);
    glBufferData(GL_ARRAY_BUFFER, m_StarCount * sizeof(float), m_StarTwinkleOffsets.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

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

    glBindVertexArray(0);
    glDisable(GL_DEPTH_TEST);
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE);

    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glVertexAttribPointer(m_PositionLoc, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(m_PositionLoc);

    glBindBuffer(GL_ARRAY_BUFFER, m_ColorVBO);
    glVertexAttribPointer(m_ColorLoc, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(m_ColorLoc);

    glBindBuffer(GL_ARRAY_BUFFER, m_SizeVBO);
    glVertexAttribPointer(m_SizeLoc, 1, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(m_SizeLoc);

    glBindBuffer(GL_ARRAY_BUFFER, m_TwinkleVBO);
    glVertexAttribPointer(m_TwinkleOffsetLoc, 1, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(m_TwinkleOffsetLoc);

    glDrawArrays(GL_POINTS, 0, m_StarCount);

    glDisableVertexAttribArray(m_PositionLoc);
    glDisableVertexAttribArray(m_ColorLoc);
    glDisableVertexAttribArray(m_SizeLoc);
    glDisableVertexAttribArray(m_TwinkleOffsetLoc);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
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
    if (m_ColorVBO != 0) {
        glDeleteBuffers(1, &m_ColorVBO);
        m_ColorVBO = 0;
    }
    if (m_SizeVBO != 0) {
        glDeleteBuffers(1, &m_SizeVBO);
        m_SizeVBO = 0;
    }
    if (m_TwinkleVBO != 0) {
        glDeleteBuffers(1, &m_TwinkleVBO);
        m_TwinkleVBO = 0;
    }
}

void StarLayer::SetParamFloat(LayerParamType paramType, float value) {
    switch (paramType) {
        case PARAM_STAR_DENSITY:
            m_StarCount = static_cast<int>(value * 500);
            GenerateStars();
            break;
        case PARAM_TIME_SPEED:
            m_TwinkleSpeed = value;
            break;
        default:
            break;
    }
}

void StarLayer::SetParamVec3(LayerParamType paramType, float x, float y, float z) {
    // StarLayer 不需要 vec3 参数
}