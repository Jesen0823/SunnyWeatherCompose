#ifndef GLLIB_COMPOSITERENDERER_H
#define GLLIB_COMPOSITERENDERER_H

#include "base/GLLayerBase.h"
#include <vector>
#include <map>
#include <string>

/**
 * 组合渲染器
 * 
 * 负责管理多个渲染层，按顺序渲染各层，支持层的添加、移除和清空操作。
 * 
 * 渲染顺序：
 * 1. SkyBackgroundLayer（天空背景层）
 * 2. CloudLayer（云层）
 * 3. PrecipitationLayer（降水层）
 * 4. ParticleLayer（颗粒层）
 * 5. EffectLayer（特效层）
 * 
 * 层堆栈按照 LayerType 自动排序，确保渲染顺序正确。
 */
class CompositeRenderer : public GLRendererBase {
public:
    CompositeRenderer();
    virtual ~CompositeRenderer();
    
    /**
     * 初始化渲染器
     */
    virtual bool Init() override;
    
    /**
     * 绘制所有层
     * @param screenW 屏幕宽度
     * @param screenH 屏幕高度
     */
    virtual void Draw(int screenW, int screenH) override;
    
    /**
     * 销毁所有层资源
     */
    virtual void Destroy() override;
    
    /**
     * 添加渲染层
     * @param layer 要添加的层（所有权转移）
     */
    void AddLayer(GLLayerBase *layer);
    
    /**
     * 移除渲染层
     * @param layerType 要移除的层类型
     */
    void RemoveLayer(LayerType layerType);
    
    /**
     * 清空所有层
     */
    void ClearLayers();
    
    /**
     * 获取指定类型的层
     * @param layerType 层类型
     * @return 层指针，不存在返回 nullptr
     */
    GLLayerBase *GetLayer(LayerType layerType);
    
    /**
     * 检查是否存在指定类型的层
     * @param layerType 层类型
     * @return 是否存在
     */
    bool HasLayer(LayerType layerType);
    
    /**
     * 获取所有层的数量
     * @return 层数量
     */
    int GetLayerCount() const;
    
    /**
     * 根据 skycon 配置层组合
     * @param skycon 天气类型字符串
     */
    void ConfigureLayers(const char *skycon);
    
private:
    /**
     * 对层堆栈进行排序，确保正确的渲染顺序
     */
    void SortLayers();
    
    /**
     * 配置晴天效果
     * @param isNight 是否夜间
     */
    void ConfigureClear(bool isNight);
    
    /**
     * 配置多云效果
     * @param isNight 是否夜间
     */
    void ConfigurePartlyCloudy(bool isNight);
    
    /**
     * 配置阴天效果
     * @param isNight 是否夜间
     */
    void ConfigureCloudy(bool isNight);
    
    /**
     * 配置雾霾效果
     * @param level 雾霾等级（0=轻度，1=中度，2=重度）
     * @param isNight 是否夜间
     */
    void ConfigureHaze(int level, bool isNight);
    
    /**
     * 配置雨效果
     * @param level 雨量等级（0=小雨，1=中雨，2=大雨，3=暴雨）
     * @param isNight 是否夜间
     */
    void ConfigureRain(int level, bool isNight);
    
    /**
     * 配置雾效果
     * @param isNight 是否夜间
     */
    void ConfigureFog(bool isNight);
    
    /**
     * 配置雪效果
     * @param level 雪量等级（0=小雪，1=中雪，2=大雪，3=暴雪）
     * @param isNight 是否夜间
     */
    void ConfigureSnow(int level, bool isNight);
    
    /**
     * 配置浮尘效果
     * @param isNight 是否夜间
     */
    void ConfigureDust(bool isNight);
    
    /**
     * 配置沙尘效果
     * @param isNight 是否夜间
     */
    void ConfigureSand(bool isNight);
    
    /**
     * 配置大风效果
     * @param isNight 是否夜间
     */
    void ConfigureWind(bool isNight);
    
    std::vector<GLLayerBase *> m_Layers;  // 层堆栈
    std::map<LayerType, GLLayerBase *> m_LayerMap;  // 层类型到层的映射
    
    int m_ScreenWidth;                    // 屏幕宽度
    int m_ScreenHeight;                   // 屏幕高度
    
    bool m_IsInitialized;                 // 是否已初始化
};

#endif // GLLIB_COMPOSITERENDERER_H