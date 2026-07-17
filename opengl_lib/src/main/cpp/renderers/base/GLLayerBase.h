#ifndef GLLIB_GLLAYERBASE_H
#define GLLIB_GLLAYERBASE_H

#include "GLRendererBase.h"

/**
 * Layer 类型枚举定义
 * 
 * 定义了天气动效系统中的所有渲染层类型，用于层堆栈管理和渲染顺序控制
 */
enum LayerType {
    LAYER_TYPE_SKY_BACKGROUND = 0,    // 天空背景层
    LAYER_TYPE_STAR = 1,               // 星星层
    LAYER_TYPE_CLOUD = 2,              // 云层
    LAYER_TYPE_PRECIPITATION = 3,      // 降水层（雨/雪）
    LAYER_TYPE_PARTICLE = 4,           // 颗粒层（雾霾/雾/沙尘）
    LAYER_TYPE_EFFECT = 5              // 特效层（闪电/风力线条）
};

/**
 * 参数类型枚举定义
 * 
 * 定义了各 Layer 支持的参数类型，用于 SetParams() 方法的参数传递
 */
enum LayerParamType {
    // 通用参数
    PARAM_TIME_OF_DAY = 100,           // 日夜模式（0=白天，1=夜间）
    PARAM_SCREEN_SIZE = 101,           // 屏幕尺寸
    PARAM_TIME = 102,                  // 时间参数（动画进度）
    PARAM_TIME_SPEED = 103,            // 时间速度（动画速度系数）
    
    // SkyBackgroundLayer 参数
    PARAM_SKY_COLOR_TOP = 200,         // 天空顶部颜色
    PARAM_SKY_COLOR_BOTTOM = 201,      // 天空底部颜色
    PARAM_SUN_INTENSITY = 202,         // 太阳亮度
    PARAM_MOON_INTENSITY = 203,        // 月亮亮度
    PARAM_STAR_DENSITY = 204,          // 星星密度
    
    // CloudLayer 参数
    PARAM_CLOUD_COVERAGE = 300,        // 云层覆盖率（0.0~1.0）
    PARAM_CLOUD_DARKNESS = 301,        // 云层暗度（0.0~1.0）
    PARAM_CLOUD_LIGHTNESS = 302,       // 云层亮部系数（0.0~1.0）
    PARAM_CLOUD_SPEED = 303,           // 云层移动速度（0.0~1.0）
    PARAM_CLOUD_SCALE = 304,           // 云层缩放（0.5~2.0）
    PARAM_CLOUD_ALPHA = 305,           // 云层透明度系数
    
    // PrecipitationLayer 参数
    PARAM_PRECIPITATION_TYPE = 400,    // 降水类型（0=雨，1=雪）
    PARAM_PRECIPITATION_INTENSITY = 401, // 降水强度（0.0~1.0）
    PARAM_PRECIPITATION_SPEED = 402,   // 下落速度（0.0~1.0）
    PARAM_PRECIPITATION_SIZE = 403,    // 雨滴/雪花大小（0.5~2.0）
    
    // ParticleLayer 参数
    PARAM_PARTICLE_TYPE = 500,         // 颗粒类型（0=雾霾，1=雾，2=浮尘，3=沙尘）
    PARAM_PARTICLE_DENSITY = 501,      // 颗粒密度（0.0~1.0）
    PARAM_PARTICLE_COLOR_R = 502,      // 颗粒颜色 R 分量
    PARAM_PARTICLE_COLOR_G = 503,      // 颗粒颜色 G 分量
    PARAM_PARTICLE_COLOR_B = 504,      // 颗粒颜色 B 分量
    PARAM_PARTICLE_VISIBILITY = 505,   // 能见度（0.0~1.0）
    
    // EffectLayer 参数
    PARAM_EFFECT_LIGHTNING_ENABLED = 600,    // 闪电启用（0=关闭，1=开启）
    PARAM_EFFECT_LIGHTNING_INTERVAL = 601,   // 闪电间隔（秒）
    PARAM_EFFECT_WIND_LINES_ENABLED = 602,   // 风力线条启用（0=关闭，1=开启）
    PARAM_EFFECT_WIND_STRENGTH = 603         // 风力强度（0.0~1.0）
};

/**
 * 层基类
 * 
 * 继承自 GLRendererBase，为天气动效系统提供统一的层接口
 * 所有渲染层（SkyBackgroundLayer、CloudLayer、PrecipitationLayer 等）
 * 都必须继承此类并实现其虚函数
 */
class GLLayerBase : public GLRendererBase {
protected:
    LayerType m_LayerType;             // 当前层类型
    bool m_Enabled;                    // 是否启用该层
    
public:
    /**
     * 构造函数
     * @param layerType 层类型
     */
    GLLayerBase(LayerType layerType) : m_LayerType(layerType), m_Enabled(true) {}
    
    /**
     * 析构函数
     */
    virtual ~GLLayerBase() {}
    
    /**
     * 获取层类型
     * @return 层类型枚举值
     */
    LayerType GetLayerType() const { return m_LayerType; }
    
    /**
     * 设置层启用状态
     * @param enabled 是否启用
     */
    void SetEnabled(bool enabled) { m_Enabled = enabled; }
    
    /**
     * 获取层启用状态
     * @return 是否启用
     */
    bool IsEnabled() const { return m_Enabled; }
    
    /**
     * 设置整数参数
     * @param paramType 参数类型
     * @param value 参数值
     */
    virtual void SetParamInt(LayerParamType paramType, int value) {}
    
    /**
     * 设置浮点参数
     * @param paramType 参数类型
     * @param value 参数值
     */
    virtual void SetParamFloat(LayerParamType paramType, float value) {}
    
    /**
     * 设置向量参数（2D）
     * @param paramType 参数类型
     * @param x X 分量
     * @param y Y 分量
     */
    virtual void SetParamVec2(LayerParamType paramType, float x, float y) {}
    
    /**
     * 设置向量参数（3D）
     * @param paramType 参数类型
     * @param x X 分量
     * @param y Y 分量
     * @param z Z 分量
     */
    virtual void SetParamVec3(LayerParamType paramType, float x, float y, float z) {}
    
    /**
     * 设置向量参数（4D）
     * @param paramType 参数类型
     * @param x X 分量
     * @param y Y 分量
     * @param z Z 分量
     * @param w W 分量
     */
    virtual void SetParamVec4(LayerParamType paramType, float x, float y, float z, float w) {}
};

#endif // GLLIB_GLLAYERBASE_H