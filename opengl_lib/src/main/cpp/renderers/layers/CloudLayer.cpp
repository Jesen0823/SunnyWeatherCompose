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
      m_CloudMode(0),
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
      m_CloudAlphaLoc(GL_NONE) {
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

    // 片段着色器：程序化云朵效果（透明叠加，背景由 SkyBackgroundLayer 提供）
    char fShaderStr[] =
            "#version 300 es                                                          \n"
            "precision mediump float;                                                 \n"
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
            "uniform int u_cloudMode;                                                 \n"
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
            "    float total = 0.0, amplitude = 0.22;                                 \n"
            "    float frequency = 1.0;                                               \n"
            "    for (int i = 0; i < 5; i++) {                                        \n"
            "        total += noise(n * frequency) * amplitude;                        \n"
            "        frequency *= 2.0;                                                 \n"
            "        amplitude *= 0.48;                                               \n"
            "    }                                                                    \n"
            "    return total;                                                        \n"
            "}                                                                        \n"
            "void main() {                                                            \n"
            "    vec2 fragCoord = gl_FragCoord.xy;                                    \n"
            "    vec2 p = fragCoord.xy / u_screenSize.xy;                             \n"
            "    vec2 uv = p*vec2(u_screenSize.x/u_screenSize.y,1.0);                 \n"
            "    float time = u_time * u_cloudSpeed;                                  \n"
            "    float q = fbm(uv * u_cloudScale * 0.3);                              \n"
            "    float r = 0.0;                                                       \n"
            "    uv *= u_cloudScale;                                                  \n"
            "    uv -= q - time * 0.5;                                                \n"
            "    float weight = 0.85;                                                 \n"
            "    for (int i=0; i<6; i++){                                             \n"
            "        r += abs(weight*noise( uv ));                                    \n"
            "        uv = m*uv + time * 0.3;                                          \n"
            "        weight *= 0.72;                                                  \n"
            "    }                                                                    \n"
            "    float f = 0.0;                                                       \n"
            "    uv = p*vec2(u_screenSize.x/u_screenSize.y,1.0);                      \n"
            "    uv *= u_cloudScale * 1.2;                                            \n"
            "    uv -= q - time * 0.7;                                                \n"
            "    weight = 0.75;                                                       \n"
            "    for (int i=0; i<6; i++){                                             \n"
            "        f += weight*noise( uv );                                         \n"
            "        uv = m*uv + time * 0.5;                                          \n"
            "        weight *= 0.62;                                                  \n"
            "    }                                                                    \n"
            "    f *= r * 1.5 + f * 0.8;                                              \n"
            "    float c = 0.0;                                                       \n"
            "    time = u_time * u_cloudSpeed * 2.0;                                  \n"
            "    uv = p*vec2(u_screenSize.x/u_screenSize.y,1.0);                      \n"
            "    uv *= u_cloudScale*1.8;                                              \n"
            "    uv -= q - time;                                                      \n"
            "    weight = 0.45;                                                       \n"
            "    for (int i=0; i<5; i++){                                             \n"
            "        c += weight*noise(uv);                                           \n"
            "        uv = m*uv + time;                                                \n"
            "        weight *= 0.65;                                                  \n"
            "    }                                                                    \n"
            "    float c1 = 0.0;                                                      \n"
            "    time = u_time * u_cloudSpeed * 3.0;                                  \n"
            "    uv = p*vec2(u_screenSize.x/u_screenSize.y,1.0);                      \n"
            "    uv *= u_cloudScale*2.5;                                              \n"
            "    uv -= q - time;                                                      \n"
            "    weight = 0.45;                                                       \n"
            "    for (int i=0; i<5; i++){                                             \n"
            "        c1 += abs(weight*noise( uv ));                                   \n"
            "        uv = m*uv + time;                                                \n"
            "        weight *= 0.65;                                                  \n"
            "    }                                                                    \n"
            "    c += c1;                                                             \n"
            "    float cloudBase = f * 0.6 + c * 0.4;                                 \n"
            "    float cloudBrightness = clamp(u_cloudDarkness + u_cloudLightness * cloudBase, 0.0, 1.0);\n"
            "    float cloudShading = clamp(f * 0.7 + c * 0.3, 0.0, 1.0);             \n"
            "    vec2 gradientUv = p*vec2(u_screenSize.x/u_screenSize.y,1.0) * 0.5;  \n"
            "    float heightNoise = fbm(gradientUv);                                 \n"
            "    float shadow = 1.0 - heightNoise * 0.3;                              \n"
            "    vec3 dayCloudColor;                                                   \n"
            "    vec3 nightCloudColor;                                                  \n"
            "    if (u_cloudMode == 1) {                                               \n"
            "        dayCloudColor = mix(vec3(0.20, 0.23, 0.28), vec3(0.45, 0.48, 0.53), cloudShading);\n"
            "        dayCloudColor *= cloudBrightness * 1.0;                            \n"
            "        nightCloudColor = mix(vec3(0.15, 0.18, 0.23), vec3(0.35, 0.38, 0.43), cloudShading);\n"
            "        nightCloudColor *= cloudBrightness * 0.9;                          \n"
            "    } else if (u_cloudMode == 2) {                                        \n"
            "        dayCloudColor = mix(vec3(0.45, 0.48, 0.55), vec3(0.80, 0.82, 0.88), cloudShading);\n"
            "        dayCloudColor *= cloudBrightness * 0.95;                           \n"
            "        nightCloudColor = mix(vec3(0.30, 0.33, 0.40), vec3(0.55, 0.58, 0.65), cloudShading);\n"
            "        nightCloudColor *= cloudBrightness * 0.85;                         \n"
            "    } else {                                                              \n"
            "        dayCloudColor = mix(vec3(0.55, 0.58, 0.65), vec3(0.98, 0.99, 1.0), cloudShading);\n"
            "        dayCloudColor *= cloudBrightness;                                  \n"
            "        nightCloudColor = vec3(0.75, 0.78, 0.85) * cloudBrightness;       \n"
            "        nightCloudColor = mix(vec3(0.4, 0.45, 0.55), nightCloudColor, cloudShading);\n"
            "    }                                                                     \n"
            "    vec3 cloudColour = mix(dayCloudColor, nightCloudColor, float(u_isNight));\n"
            "    cloudColour *= shadow;                                               \n"
            "    float baseAlpha = f * 0.9 + c * 0.7;                                  \n"
            "    float nightAlphaMultiplier = u_isNight == 1 ? 0.9 : 1.0;             \n"
            "    float alphaFactor = u_cloudAlpha / 20.0;                              \n"
            "    float cloudAlpha = u_cloudCoverage * baseAlpha * nightAlphaMultiplier * alphaFactor;\n"
            "    cloudAlpha = clamp(cloudAlpha, 0.0, 1.0);                            \n"
            "    float edgeSoftening = smoothstep(0.0, 0.25, cloudAlpha);             \n"
            "    cloudAlpha *= edgeSoftening;                                         \n"
            "    outColor = vec4(cloudColour, cloudAlpha);                            \n"
            "}";

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
    m_CloudModeLoc = glGetUniformLocation(m_ProgramObj, "u_cloudMode");

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
    glUniform1i(m_CloudModeLoc, m_CloudMode);

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
        case PARAM_CLOUD_MODE:
            m_CloudMode = value;
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