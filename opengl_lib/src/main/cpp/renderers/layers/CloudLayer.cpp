#include "CloudLayer.h"
#include "../../util/GLUtils.h"
#include "../../util/LogUtil.h"
#include "gtc/matrix_transform.hpp"

CloudLayer::CloudLayer() 
    : GLLayerBase(LAYER_TYPE_CLOUD),
      m_CloudCoverage(0.85f),
      m_CloudDarkness(0.80f),
      m_CloudLightness(1.0f),
      m_CloudSpeed(0.01f),
      m_CloudScale(0.7f),
      m_CloudAlpha(20.0f),
      m_IsNight(false),
      m_ScreenWidth(0),
      m_ScreenHeight(0),
      m_Time(0.0f),
      m_FrameIndex(0),
      m_VaoId(GL_NONE),
      m_MVPMatLoc(GL_NONE),
      m_TimeLoc(GL_NONE),
      m_ScreenSizeLoc(GL_NONE),
      m_CloudCoverageLoc(GL_NONE),
      m_CloudDarknessLoc(GL_NONE),
      m_CloudLightnessLoc(GL_NONE),
      m_CloudSpeedLoc(GL_NONE),
      m_CloudScaleLoc(GL_NONE),
      m_CloudAlphaLoc(GL_NONE),
      m_IsNightLoc(GL_NONE) {
    m_VboIds[0] = m_VboIds[1] = m_VboIds[2] = GL_NONE;
}

CloudLayer::~CloudLayer() {}

bool CloudLayer::Init() {
    LOGCATI("CloudLayer::Init: called, m_ProgramObj=%d", m_ProgramObj);
    if (m_ProgramObj) {
        LOGCATI("CloudLayer::Init: already initialized, skipping");
        return true;
    }

    // 顶点着色器
    char vShaderStr[] =
            "#version 300 es                                       \n"
            "layout(location = 0) in vec4 a_position;              \n"
            "uniform mat4 u_MVPMatrix;                             \n"
            "void main(){                                          \n"
            "    gl_Position = u_MVPMatrix * a_position;           \n"
            "}";

    // 片段着色器：程序化云朵效果（参考 CloudRenderer.cpp 的简洁实现，透明叠加，背景由 SkyBackgroundLayer 提供）
    char fShaderStr[] =
            "#version 300 es                                                          \n"
            "precision highp float;                                                   \n"
            "layout(location = 0) out vec4 outColor;                                  \n"
            "uniform float u_time;                                                    \n"
            "uniform vec2 u_screenSize;                                               \n"
            "uniform float u_cloudCoverage;                                           \n"
            "uniform float u_cloudDarkness;                                           \n"
            "uniform float u_cloudLightness;                                          \n"
            "uniform float u_cloudSpeed;                                              \n"
            "uniform float u_cloudScale;                                              \n"
            "uniform float u_cloudAlpha;                                              \n"
            "uniform int u_isNight;                                                   \n"
            "const mat2 m = mat2( 1.6,  1.2, -1.2,  1.6 );                            \n"
            "vec2 hash( vec2 p ) {                                                    \n"
            "    p = vec2(dot(p,vec2(127.1,311.7)), dot(p,vec2(269.5,183.3)));        \n"
            "    return -1.0 + 2.0*fract(sin(p)*43758.5453123);                       \n"
            "}                                                                        \n"
            "float noise( in vec2 p ) {                                               \n"
            "    const float K1 = 0.366025404;                                         \n"
            "    const float K2 = 0.211324865;                                         \n"
            "    vec2 i = floor(p + (p.x+p.y)*K1);                                    \n"
            "    vec2 a = p - i + (i.x+i.y)*K2;                                       \n"
            "    vec2 o = (a.x>a.y) ? vec2(1.0,0.0) : vec2(0.0,1.0);                 \n"
            "    vec2 b = a - o + K2;                                                 \n"
            "    vec2 c = a - 1.0 + 2.0*K2;                                           \n"
            "    vec3 h = max(0.5-vec3(dot(a,a), dot(b,b), dot(c,c) ), 0.0 );         \n"
            "    vec3 n = h*h*h*h*vec3( dot(a,hash(i+0.0)), dot(b,hash(i+o)), dot(c,hash(i+1.0)));\n"
            "    return dot(n, vec3(70.0));                                           \n"
            "}                                                                        \n"
            "float fbm(vec2 n) {                                                      \n"
            "    float total = 0.0, amplitude = 0.1;                                  \n"
            "    for (int i = 0; i < 7; i++) {                                        \n"
            "        total += noise(n) * amplitude;                                   \n"
            "        n = m * n;                                                       \n"
            "        amplitude *= 0.4;                                                \n"
            "    }                                                                    \n"
            "    return total;                                                        \n"
            "}                                                                        \n"
            "void main() {                                                            \n"
            "    vec2 fragCoord = gl_FragCoord.xy;                                    \n"
            "    vec2 p = fragCoord.xy / u_screenSize.xy;                             \n"
            "    vec2 uv = p*vec2(u_screenSize.x/u_screenSize.y,1.0);                 \n"
            "    float time = u_time * u_cloudSpeed;                                  \n"
            "    float q = fbm(uv * u_cloudScale * 0.5);                              \n"
            "    float r = 0.0;                                                       \n"
            "    uv *= u_cloudScale;                                                  \n"
            "    uv -= q - time;                                                      \n"
            "    float weight = 0.8;                                                  \n"
            "    for (int i=0; i<8; i++){                                             \n"
            "        r += abs(weight*noise( uv ));                                    \n"
            "        uv = m*uv + time;                                                \n"
            "        weight *= 0.7;                                                   \n"
            "    }                                                                    \n"
            "    float f = 0.0;                                                       \n"
            "    uv = p*vec2(u_screenSize.x/u_screenSize.y,1.0);                      \n"
            "    uv *= u_cloudScale;                                                  \n"
            "    uv -= q - time;                                                      \n"
            "    weight = 0.7;                                                        \n"
            "    for (int i=0; i<8; i++){                                             \n"
            "        f += weight*noise( uv );                                         \n"
            "        uv = m*uv + time;                                                \n"
            "        weight *= 0.6;                                                   \n"
            "    }                                                                    \n"
            "    f *= r + f;                                                          \n"
            "    float c = 0.0;                                                       \n"
            "    time = u_time * u_cloudSpeed * 2.0;                                  \n"
            "    uv = p*vec2(u_screenSize.x/u_screenSize.y,1.0);                      \n"
            "    uv *= u_cloudScale*2.0;                                              \n"
            "    uv -= q - time;                                                      \n"
            "    weight = 0.4;                                                        \n"
            "    for (int i=0; i<7; i++){                                             \n"
            "        c += weight*noise(uv);                                           \n"
            "        uv = m*uv + time;                                                \n"
            "        weight *= 0.6;                                                   \n"
            "    }                                                                    \n"
            "    float c1 = 0.0;                                                      \n"
            "    time = u_time * u_cloudSpeed * 3.0;                                  \n"
            "    uv = p*vec2(u_screenSize.x/u_screenSize.y,1.0);                      \n"
            "    uv *= u_cloudScale*3.0;                                              \n"
            "    uv -= q - time;                                                      \n"
            "    weight = 0.4;                                                        \n"
            "    for (int i=0; i<7; i++){                                             \n"
            "        c1 += abs(weight*noise( uv ));                                   \n"
            "        uv = m*uv + time;                                                \n"
            "        weight *= 0.6;                                                   \n"
            "    }                                                                    \n"
            "    c += c1;                                                             \n"
            "    vec3 cloudColour = mix(vec3(0.88, 0.90, 0.96), vec3(1.0, 1.0, 1.0), clamp(c * 1.5, 0.0, 1.0));\n"

            "    f = u_cloudCoverage + u_cloudAlpha*f*r;                              \n"
            "    float cloudAlpha = clamp(f + c, 0.0, 1.0);                           \n"
            "    float nightAlpha = u_isNight == 1 ? 0.8 : 1.0;                       \n"
            "    cloudAlpha *= nightAlpha;                                            \n"
            "    vec3 nightTint = mix(vec3(0.15, 0.18, 0.25), vec3(0.35, 0.38, 0.48), c);\n"
            "    cloudColour = mix(cloudColour, nightTint, float(u_isNight));         \n"
            "    outColor = vec4(cloudColour, cloudAlpha);                            \n"
            "}                                                                        \n";

    m_ProgramObj = GLUtils::CreateProgram(vShaderStr, fShaderStr, m_VertexShader, m_FragmentShader);
    if (!m_ProgramObj) {
        LOGCATE("CloudLayer::Init create program fail");
        return false;
    }

    m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
    m_TimeLoc = glGetUniformLocation(m_ProgramObj, "u_time");
    m_ScreenSizeLoc = glGetUniformLocation(m_ProgramObj, "u_screenSize");
    m_CloudCoverageLoc = glGetUniformLocation(m_ProgramObj, "u_cloudCoverage");
    m_CloudDarknessLoc = glGetUniformLocation(m_ProgramObj, "u_cloudDarkness");
    m_CloudLightnessLoc = glGetUniformLocation(m_ProgramObj, "u_cloudLightness");
    m_CloudSpeedLoc = glGetUniformLocation(m_ProgramObj, "u_cloudSpeed");
    m_CloudScaleLoc = glGetUniformLocation(m_ProgramObj, "u_cloudScale");
    m_CloudAlphaLoc = glGetUniformLocation(m_ProgramObj, "u_cloudAlpha");
    m_IsNightLoc = glGetUniformLocation(m_ProgramObj, "u_isNight");

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
    LOGCATI("CloudLayer::Init success, m_ProgramObj = %d", m_ProgramObj);
    return true;
}

void CloudLayer::Draw(int screenW, int screenH) {
    if (!m_ProgramObj || !m_Enabled) {
        return;
    }

    if (m_ScreenWidth != screenW || m_ScreenHeight != screenH) {
        m_ScreenWidth = screenW;
        m_ScreenHeight = screenH;
        UpdateMVPMatrix();
    }

    glUseProgram(m_ProgramObj);

    glBindVertexArray(m_VaoId);

    m_FrameIndex++;
    m_Time = m_FrameIndex * 0.04f;

    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    glUniform1f(m_TimeLoc, m_Time);
    glUniform2f(m_ScreenSizeLoc, screenW, screenH);
    glUniform1f(m_CloudCoverageLoc, m_CloudCoverage);
    glUniform1f(m_CloudDarknessLoc, m_CloudDarkness);
    glUniform1f(m_CloudLightnessLoc, m_CloudLightness);
    glUniform1f(m_CloudSpeedLoc, m_CloudSpeed);
    glUniform1f(m_CloudScaleLoc, m_CloudScale);
    glUniform1f(m_CloudAlphaLoc, m_CloudAlpha);
    glUniform1i(m_IsNightLoc, m_IsNight ? 1 : 0);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, (const void *) 0);

    glDisable(GL_BLEND);

    glBindVertexArray(GL_NONE);
}

void CloudLayer::Destroy() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
        glDeleteBuffers(3, m_VboIds);
        glDeleteVertexArrays(1, &m_VaoId);
        m_ProgramObj = GL_NONE;
    }
}

void CloudLayer::SetParamInt(LayerParamType paramType, int value) {
    switch (paramType) {
        case PARAM_TIME_OF_DAY:
            m_IsNight = (value == 1);
            break;
        default:
            break;
    }
}

void CloudLayer::SetParamFloat(LayerParamType paramType, float value) {
    switch (paramType) {
        case PARAM_CLOUD_COVERAGE:
            m_CloudCoverage = value;
            break;
        case PARAM_CLOUD_DARKNESS:
            m_CloudDarkness = value;
            break;
        case PARAM_CLOUD_LIGHTNESS:
            m_CloudLightness = value;
            break;
        case PARAM_CLOUD_SPEED:
            m_CloudSpeed = value;
            break;
        case PARAM_CLOUD_SCALE:
            m_CloudScale = value;
            break;
        case PARAM_CLOUD_ALPHA:
            m_CloudAlpha = value;
            break;
        default:
            break;
    }
}

void CloudLayer::UpdateMVPMatrix() {
    float ratio = (float) m_ScreenWidth / m_ScreenHeight;

    glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    glm::mat4 View = glm::lookAt(glm::vec3(0, 0, 4), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 Model = glm::mat4(1.0f);

    m_MVPMatrix = Projection * View * Model;
}