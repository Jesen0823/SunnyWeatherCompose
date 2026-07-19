赶着假期的尾巴整个活，在 WebGL2 上实现了一个雨打屏幕的效果。本文简单记载一下实现的思路，踩的坑以及一些[优化方法](https://zhida.zhihu.com/search?content_id=166711542&content_type=Article&match_order=1&q=%E4%BC%98%E5%8C%96%E6%96%B9%E6%B3%95&zd_token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJ6aGlkYV9zZXJ2ZXIiLCJleHAiOjE3ODQ1NTM2MjUsInEiOiLkvJjljJbmlrnms5UiLCJ6aGlkYV9zb3VyY2UiOiJlbnRpdHkiLCJjb250ZW50X2lkIjoxNjY3MTE1NDIsImNvbnRlbnRfdHlwZSI6IkFydGljbGUiLCJtYXRjaF9vcmRlciI6MSwiemRfdG9rZW4iOm51bGx9.Prww78Ts7BVyEvqvfVh2lxbEtTuQJ1w3tBh6hAIPkBU&zhida_source=entity)。效果预览：

![](%E7%94%A8webgl%E5%AE%9E%E7%8E%B0%E9%9B%A8%E6%89%93%E5%B1%8F%E5%B9%95/v2-e90d137df38ec3e27c8fe4d5614b461c_1440w.jpg)

效果图

_图中所用背景来自 [https://www.pixiv.net/artworks/84765992](https://link.zhihu.com/?target=https%3A//www.pixiv.net/artworks/84765992)_

雨滴算是一种比较常见的特效，[Shadertoy](https://zhida.zhihu.com/search?content_id=166711542&content_type=Article&match_order=1&q=Shadertoy&zd_token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJ6aGlkYV9zZXJ2ZXIiLCJleHAiOjE3ODQ1NTM2MjUsInEiOiJTaGFkZXJ0b3kiLCJ6aGlkYV9zb3VyY2UiOiJlbnRpdHkiLCJjb250ZW50X2lkIjoxNjY3MTE1NDIsImNvbnRlbnRfdHlwZSI6IkFydGljbGUiLCJtYXRjaF9vcmRlciI6MSwiemRfdG9rZW4iOm51bGx9.JxdDq4KZVo-hshMolm0Onhd1TXKtp0n90UtQGognBC0&zhida_source=entity) 上能找到不少纯 Shader 的实现，例如 [https://www.shadertoy.com/view/MdfBRX](https://link.zhihu.com/?target=https%3A//www.shadertoy.com/view/MdfBRX)， [https://www.shadertoy.com/view/tlVGWK](https://link.zhihu.com/?target=https%3A//www.shadertoy.com/view/tlVGWK)，[https://www.shadertoy.com/view/tlGcWG](https://link.zhihu.com/?target=https%3A//www.shadertoy.com/view/tlGcWG)。

但这些效果最大的瑕疵在于水滴之间没有任何[交互作用](https://zhida.zhihu.com/search?content_id=166711542&content_type=Article&match_order=1&q=%E4%BA%A4%E4%BA%92%E4%BD%9C%E7%94%A8&zd_token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJ6aGlkYV9zZXJ2ZXIiLCJleHAiOjE3ODQ1NTM2MjUsInEiOiLkuqTkupLkvZznlKgiLCJ6aGlkYV9zb3VyY2UiOiJlbnRpdHkiLCJjb250ZW50X2lkIjoxNjY3MTE1NDIsImNvbnRlbnRfdHlwZSI6IkFydGljbGUiLCJtYXRjaF9vcmRlciI6MSwiemRfdG9rZW4iOm51bGx9.41lsAWgEx2_nyyGRdUyYfbLd5D_DvREWjL98bTItyg4&zhida_source=entity)，而现实中滑落的水滴会与其他水滴凝聚融合成一个更大的水滴加速滑落。这个复杂的逻辑很难单纯使用 Shader 实现，而结合 CPU 实现的雨滴效果个人最喜欢的是 [Lucas Bebber](https://link.zhihu.com/?target=https%3A//tympanus.net/codrops/author/lucasbebber/) 的 [Rain & Water Effect Experiments](https://link.zhihu.com/?target=https%3A//tympanus.net/codrops/2015/11/04/rain-water-effect-experiments/)，但 Lucas Bebber 的实验性实现性能上存在缺陷，并且难以移植和复用，遂寻思着复刻一个。

![](./v2-01.jpg)

Lucas Bebber 的 Rain &amp;amp;amp;amp;amp; Water Effect Experiments 效果图

阅读本文需要一定的[图形学](https://zhida.zhihu.com/search?content_id=166711542&content_type=Article&match_order=1&q=%E5%9B%BE%E5%BD%A2%E5%AD%A6&zd_token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJ6aGlkYV9zZXJ2ZXIiLCJleHAiOjE3ODQ1NTM2MjUsInEiOiLlm77lvaLlraYiLCJ6aGlkYV9zb3VyY2UiOiJlbnRpdHkiLCJjb250ZW50X2lkIjoxNjY3MTE1NDIsImNvbnRlbnRfdHlwZSI6IkFydGljbGUiLCJtYXRjaF9vcmRlciI6MSwiemRfdG9rZW4iOm51bGx9.u7Wy1Q_tRZWb1QomMVPSpZTbBrGPq2svrUWx7G0z-b0&zhida_source=entity)和渲染管线前置知识，文中代码所用渲染器的接口设计和名称与 Unity 近似。

文中的代码为 TypeScript 语法的简化代码，仅用于辅助理解，实际代码参考 [GitHub](https://github.com/SardineFish/raindrop-fx)。

## Raindrop Simulation

### Size & Mass

这个效果中，需要模拟雨滴在光滑物体表面的一些物理行为，我们以每一颗独立的雨滴作为一个模拟对象，雨滴以随机的时间间隔生成并保存到一个 array 中，每帧遍历更新每一个雨滴的运动状态。模拟的结果需要将每一颗雨滴的 2D 位置 `position` 和 2D 尺寸 `size` 交给渲染器渲染。在模拟中我们假定质量与尺寸的关系为 `mass = size ^ 2`，当然线性关系或是三次方的关系也 OK，但在测试中发现平方关系更好。

附着在屏幕上的雨滴可以随时间而蒸发减少其自身的质量，并在质量归零后予以删除，以此避免大量雨滴驻留在屏幕上造成[性能压力](https://zhida.zhihu.com/search?content_id=166711542&content_type=Article&match_order=1&q=%E6%80%A7%E8%83%BD%E5%8E%8B%E5%8A%9B&zd_token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJ6aGlkYV9zZXJ2ZXIiLCJleHAiOjE3ODQ1NTM2MjUsInEiOiLmgKfog73ljovlipsiLCJ6aGlkYV9zb3VyY2UiOiJlbnRpdHkiLCJjb250ZW50X2lkIjoxNjY3MTE1NDIsImNvbnRlbnRfdHlwZSI6IkFydGljbGUiLCJtYXRjaF9vcmRlciI6MSwiemRfdG9rZW4iOm51bGx9.MyDJlH5cVwO7ks-YoOlkb44HIfMowXiVipJJ5JfaOkc&zhida_source=entity)。

在我的 Demo 中，`60/s` 的蒸发速度可以将整个屏幕的雨滴数量维持在400个左右，而无蒸发的情况下，长时间运行可能会积攒超过3000个雨滴，在有碰撞融合的情况下。

### Motion

附着在屏幕上的水滴受重力影响向下滑落，观察现实中的水滴效果可以发现，较小的水滴通常附着在表面不会发生滑落，而较大的水滴则会受阻力和重力影响随机的向下滑落或是减速附着。这里我们可以通过每隔一定的时间间隔给雨滴设置随机的阻力实现这一效果，对于不同大小的雨滴均采用同样的阻力取值范围，如此较小较轻的水滴更难以滑落，而较大较重的水滴则更容易快速滑落。除此之外真实的水滴也不会笔直地下落，我们同样可以随机地引入水平速度偏移实现这一效果。

```ts
class Raindrop
{
    pos: vec2;
    velocity: vec2;
    mass: number;
    drag: number;
    offset: number;
    update(dt: number)
    {
        const acceleration = this.mass * GRAVITY - this.drag;
        this.velocity.y -= acceleration * dt;
        this.velocity.x = this.velocity.x * this.offset;
        this.pos += this.velocity * dt;

        // ...
    }
    // Call every 0.1s
    randomMotion()
    {
        this.drag = randomRange(0, GRAVITY * MAX_MASS * 2);
        this.offset = randomRange(-0.2, 0.2);
    }
}
```

### Spread

雨滴打在表面的瞬间会向四周扩撒，随后很快又因为表面张力作用而收缩，我们可以引入一个属性 `spread` 用来描述雨滴的扩散程度，并令其随时间衰减，以实现这一效果。我们同样还可以利用这个属性让雨滴在加速下落时稍微纵向拉伸，在减速附着时纵向压缩。

```text
class Raindrop
{
    // ...
    spread: vec2;
    onSpawn()
    {
        this.spread = vec2(0.5);
    }
    update(dt: number)
    {
        // ...

        this.spread.x *= Math.pow(0.01, dt);
        this.spread.y *= Math.pow(0.01, dt);
        this.spread.y = max(this.spread.y, this.velocity.y * SPREAD_BY_VELOCITY);
        this.size = this.baseSize * (this.spread + 1);
    }
}
```

### Trail

雨滴在沿屏幕滑落时还会留下一道水迹，这道水迹又会因[表面张力](https://zhida.zhihu.com/search?content_id=166711542&content_type=Article&match_order=2&q=%E8%A1%A8%E9%9D%A2%E5%BC%A0%E5%8A%9B&zd_token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJ6aGlkYV9zZXJ2ZXIiLCJleHAiOjE3ODQ1NTM2MjUsInEiOiLooajpnaLlvKDlipsiLCJ6aGlkYV9zb3VyY2UiOiJlbnRpdHkiLCJjb250ZW50X2lkIjoxNjY3MTE1NDIsImNvbnRlbnRfdHlwZSI6IkFydGljbGUiLCJtYXRjaF9vcmRlciI6MiwiemRfdG9rZW4iOm51bGx9.yNoDwC_TOR1FTEm44nSKOn2VuLN_RfM9u-fPUgAPJcM&zhida_source=entity)作用收缩成小水珠留在表面。我们可以给正在滑落的水滴处生成新的小水滴，并加以一定的 `spread` 实现这一效果，我们可以根据滑落的速度为新生成的小水滴设置不同的 `spread.y` 值，使得快速滑落的雨滴拥有更长更连续的水迹效果。（水迹连续的效果在后文的渲染实现部分介绍）

在这里我们可以考虑[质量守恒](https://zhida.zhihu.com/search?content_id=166711542&content_type=Article&match_order=1&q=%E8%B4%A8%E9%87%8F%E5%AE%88%E6%81%92&zd_token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJ6aGlkYV9zZXJ2ZXIiLCJleHAiOjE3ODQ1NTM2MjUsInEiOiLotKjph4_lrojmgZIiLCJ6aGlkYV9zb3VyY2UiOiJlbnRpdHkiLCJjb250ZW50X2lkIjoxNjY3MTE1NDIsImNvbnRlbnRfdHlwZSI6IkFydGljbGUiLCJtYXRjaF9vcmRlciI6MSwiemRfdG9rZW4iOm51bGx9.2CfbYnNB1DJz4BDo4zsBzGK8eyzpnC6x1KJ5TBl-pow&zhida_source=entity)的原则，对从滑落的水滴中减去新生成的水滴的质量。如此一来，水滴在滑落过程中不断损失质量，尺寸也随之减小，下落的速度也随之更大概率的降低。生成的小水滴尺寸越大，则滑落的水滴质量损失越快，我们这里采用的是 `mass = size ^ 2` 的关系，很容易在水滴滑落半个屏幕就损失掉大量的水分。于是这里我们可以引入一个表示水滴厚度的属性 `density`，即我们可以认为随着尾迹生成的水滴很薄，即便看上去具有较大的尺寸，但其质量可以不大。

新生成的水滴可以在水平方向加以一定的随机偏移以提升观感。

```ts
class Raindrop
{
    // ...
    lastTrailPos: vec2;
    density: vec2;
    consturctor(density: number, size: number)
    {
        this.mass = size * size * density;
        // ...
    }
    update(dt: number)
    {
        if(distance(this.pos - this.lastTrailPos) > TrailDistance)
        {
            trailDrop = new Raindrop(0.1, this.size.x * randomRange(0.3, 0.5));
            this.mass -= trailDrop.mass;
            trailDrop.spread = vec2(0.3, this.velocity.y * TrailSpreadByVelocity);
            trailDrop.position = this.pos + vec2(randomRange(-0.3, 0.3), 0.5 * this.size.y);
            trailDrop.parent = this;
            this.lastTrailPos = this.pos;
        }
        // ...
    }
}
```

### Collision & Merge

为了提升真实感，我们需要模拟两个水滴发生碰撞时的聚合效果，朴素的实现方式可以是遍历所有雨滴进行距离计算，[时间复杂度](https://zhida.zhihu.com/search?content_id=166711542&content_type=Article&match_order=1&q=%E6%97%B6%E9%97%B4%E5%A4%8D%E6%9D%82%E5%BA%A6&zd_token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJ6aGlkYV9zZXJ2ZXIiLCJleHAiOjE3ODQ1NTM2MjUsInEiOiLml7bpl7TlpI3mnYLluqYiLCJ6aGlkYV9zb3VyY2UiOiJlbnRpdHkiLCJjb250ZW50X2lkIjoxNjY3MTE1NDIsImNvbnRlbnRfdHlwZSI6IkFydGljbGUiLCJtYXRjaF9vcmRlciI6MSwiemRfdG9rZW4iOm51bGx9.1Q_VXDDxfiytv4eNJ7VpJw0tJhXBY4BHhvr18sTDw9U&zhida_source=entity) O(N²)。当两个水滴的距离低于聚合的阈值时，取质量较大的雨滴合并他们的质量和[动量](https://zhida.zhihu.com/search?content_id=166711542&content_type=Article&match_order=1&q=%E5%8A%A8%E9%87%8F&zd_token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJ6aGlkYV9zZXJ2ZXIiLCJleHAiOjE3ODQ1NTM2MjUsInEiOiLliqjph48iLCJ6aGlkYV9zb3VyY2UiOiJlbnRpdHkiLCJjb250ZW50X2lkIjoxNjY3MTE1NDIsImNvbnRlbnRfdHlwZSI6IkFydGljbGUiLCJtYXRjaF9vcmRlciI6MSwiemRfdG9rZW4iOm51bGx9.qnL2CTSGcWXNllI0HKNShuVHmOFniv6o6AvliNTkRPs&zhida_source=entity)，删除较轻的雨滴。在此前实现滑落水迹时，我们生成的水滴由于距离较近，可能会触发聚合，因此在前文的代码示例中加入了 `trailDrop.parent` 属性以避免这一问题。

```text
collisionCheck()
{
    for(const raindrop of Raindrops)
    for(const other of Raindrops)
    {
        if (raindrop == other || raindrop.parent == other || raindrop == other.parent)
            continue;
        if(distance(raindrop.pos, other.pos) - raindrop.radius - other.radius < 0)
        {
            raindrop.mass >= other.mass
                ? raindrop.merge(other)
                : other.merge(raindrop);
        }
    }
}
class Raindrop
{
    get radius() { return this.size.x * 0.6 }
    merge(other: Raindrop)
    {
        const momentum = this.mass * this.velocity + other.mass * other.velocity;
        this.mass += other.mass;
        this.velocity = momentum / this.mass;
        other.destroy();
    }
    // ...
}
```

## Rendering

### Merging

液体效果可以通过将液体粒子化，利用诸如 [Metaballs](https://zhida.zhihu.com/search?content_id=166711542&content_type=Article&match_order=1&q=Metaballs&zd_token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJ6aGlkYV9zZXJ2ZXIiLCJleHAiOjE3ODQ1NTM2MjUsInEiOiJNZXRhYmFsbHMiLCJ6aGlkYV9zb3VyY2UiOiJlbnRpdHkiLCJjb250ZW50X2lkIjoxNjY3MTE1NDIsImNvbnRlbnRfdHlwZSI6IkFydGljbGUiLCJtYXRjaF9vcmRlciI6MSwiemRfdG9rZW4iOm51bGx9.S10ZjCDRKBaufP0rS7LVvtAI8YJ01th-dIPNmOjPvIQ&zhida_source=entity) 这类方法渲染出融合效果。Metaball 使用隐函数表示一个球体，例如：

$F(x,y,z) = metaball(x, y, z) = \frac{1}{(x-x_0)^2 + (y-y_0)^2+(z-z_0)^2} \le Threshold$

metaball 之间的融合通过加法实现，即若干个 metaball 融合后的物体[隐函数](https://zhida.zhihu.com/search?content_id=166711542&content_type=Article&match_order=2&q=%E9%9A%90%E5%87%BD%E6%95%B0&zd_token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJ6aGlkYV9zZXJ2ZXIiLCJleHAiOjE3ODQ1NTM2MjUsInEiOiLpmpDlh73mlbAiLCJ6aGlkYV9zb3VyY2UiOiJlbnRpdHkiLCJjb250ZW50X2lkIjoxNjY3MTE1NDIsImNvbnRlbnRfdHlwZSI6IkFydGljbGUiLCJtYXRjaF9vcmRlciI6MiwiemRfdG9rZW4iOm51bGx9.dNvQ-MT7eawDBgj-UbwjLe_oY_lCxJ-XThQhwoOY-RU&zhida_source=entity)表示为：

$F(x,y,z) = \Sigma_{i}metaball_i(x,y,z)$

我们将隐函数 $F(x,y,z)=Threshold$ 的[等值面](https://zhida.zhihu.com/search?content_id=166711542&content_type=Article&match_order=1&q=%E7%AD%89%E5%80%BC%E9%9D%A2&zd_token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJ6aGlkYV9zZXJ2ZXIiLCJleHAiOjE3ODQ1NTM2MjUsInEiOiLnrYnlgLzpnaIiLCJ6aGlkYV9zb3VyY2UiOiJlbnRpdHkiLCJjb250ZW50X2lkIjoxNjY3MTE1NDIsImNvbnRlbnRfdHlwZSI6IkFydGljbGUiLCJtYXRjaF9vcmRlciI6MSwiemRfdG9rZW4iOm51bGx9.-js63UxogzRrHkrC567NA3LI-8n_1UCRnfjy2UAIZeQ&zhida_source=entity)渲染出来就可以得到如下图的效果

![](./v2-02.jpg)

这一方法对于 2D 上同样适用，我们可以令

$metaball(x,y) = \frac{1}{(x-x_0)^2 + (y-y_0)^2} <= Threshold$

作为一个 2D 的 metaball 图形，同样使用加法进行混合，如下图表示了一个 $metaball(x,y)$ 的函数值，以及两个靠近的 metaball 叠加混合后的函数值：

![](./v2-03.jpg)

在这里，我们可以使用 texture 的 alpha 通道作为 metaball 的函数值，近似地使用一张径向渐变的 alpha 贴图作为 metaball，并使用一个 pass 过滤出混合后 `alpha >= threshold` 的部分

![动图封面](v2-04.webp)

过滤前后的 alpha 通道混合效果

如图是对 alpha >= 0.95 进行过滤后的结果。

![动图封面](v2-05.webp)

水滴融合的渲染效果

### Refraction

透过雨滴类似凸透镜的折射效果，我们应该可以看到中心对成的背景像。要想得到更加正确的效果，可以通过计算雨滴表面法线，依照[折射定律](https://zhida.zhihu.com/search?content_id=166711542&content_type=Article&match_order=1&q=%E6%8A%98%E5%B0%84%E5%AE%9A%E5%BE%8B&zd_token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJ6aGlkYV9zZXJ2ZXIiLCJleHAiOjE3ODQ1NTM2MjUsInEiOiLmipjlsITlrprlvosiLCJ6aGlkYV9zb3VyY2UiOiJlbnRpdHkiLCJjb250ZW50X2lkIjoxNjY3MTE1NDIsImNvbnRlbnRfdHlwZSI6IkFydGljbGUiLCJtYXRjaF9vcmRlciI6MSwiemRfdG9rZW4iOm51bGx9.lroDczkIxRBTlTH4qKT8M6L32zPCZHlUdlu2KCMF5yg&zhida_source=entity)对背景采样的 UV 进行偏折。但在这里我们使用类似 [Normal Mapping](https://zhida.zhihu.com/search?content_id=166711542&content_type=Article&match_order=1&q=Normal+Mapping&zd_token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJ6aGlkYV9zZXJ2ZXIiLCJleHAiOjE3ODQ1NTM2MjUsInEiOiJOb3JtYWwgTWFwcGluZyIsInpoaWRhX3NvdXJjZSI6ImVudGl0eSIsImNvbnRlbnRfaWQiOjE2NjcxMTU0MiwiY29udGVudF90eXBlIjoiQXJ0aWNsZSIsIm1hdGNoX29yZGVyIjoxLCJ6ZF90b2tlbiI6bnVsbH0.l6yMshTsJGnnhAM1cy4WqTQ6cCPydGII8fFSZWeDCZk&zhida_source=entity) 的方法，将如下的图与[径向模糊](https://zhida.zhihu.com/search?content_id=166711542&content_type=Article&match_order=1&q=%E5%BE%84%E5%90%91%E6%A8%A1%E7%B3%8A&zd_token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJ6aGlkYV9zZXJ2ZXIiLCJleHAiOjE3ODQ1NTM2MjUsInEiOiLlvoTlkJHmqKHns4oiLCJ6aGlkYV9zb3VyY2UiOiJlbnRpdHkiLCJjb250ZW50X2lkIjoxNjY3MTE1NDIsImNvbnRlbnRfdHlwZSI6IkFydGljbGUiLCJtYXRjaF9vcmRlciI6MSwiemRfdG9rZW4iOm51bGx9.r5vN_x5ZA7WpKM3nIY7XfAK0_C_ROwLuMdGhlIkcR9o&zhida_source=entity)的 alpha 通道混合作为雨滴的纹理，渲染到一个 RenderTexture 中，随后使用这张 RenderTexture 中 `r, g` 通道的值对 UV 进行扭曲后采样背景贴图，实现折射效果的渲染。我们不妨在后文中称下图这张混合后的 Texture 为 DistortTexture，这张 Texture 中，的中心处 `r = g = 0.5` 表示偏折量为0。

![](v2-06.jpg)

```text
raindropMaterial.Texture = DistortTexture; // ↑ DistortTexture mentioned above
renderer.setRenderTarget(raindropCompose);
for (const raindrop of Raindrops)
{
    const modelMat = mat4.rts(quat.identity(), raindrop.pos, raindrop.size);
    renderer.drawMesh(quadMesh, modelMat, raindropMaterial);
}
refractMaterial.RaindropTex = raindropCompose;
refractMaterial.Background = backgroundImage;
renderer.blit(null, CanvasOutput, refractMaterial);
```

refractMaterial 的 Shader 可以用以下代码简单概括

```glsl
uniform sampler2D RaindropTex;
uniform sampler2D Background;

in vec2 FragUV;

main()
{
    vec4 raindrop = texture(RaindropTex, FragUV.xy).rgba;
    float mask = smoothstep(0.95, 1,0, raindrop.a);
    vec2 refract = -(raindrop.xy * 2 - 1) * REFRACT_SCALE;
    vec3 color = texture(Background, FragUV.xy + refract.xy);

    fragColor = vec4(color.rgb, mask);
}
```

### Compose & Blending

前面提到我们将所有雨滴的 DistortTexture 渲染到一张 RT 中，我们需要一个合理的 [Alpha Blending](https://zhida.zhihu.com/search?content_id=166711542&content_type=Article&match_order=1&q=Alpha+Blending&zd_token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJ6aGlkYV9zZXJ2ZXIiLCJleHAiOjE3ODQ1NTM2MjUsInEiOiJBbHBoYSBCbGVuZGluZyIsInpoaWRhX3NvdXJjZSI6ImVudGl0eSIsImNvbnRlbnRfaWQiOjE2NjcxMTU0MiwiY29udGVudF90eXBlIjoiQXJ0aWNsZSIsIm1hdGNoX29yZGVyIjoxLCJ6ZF90b2tlbiI6bnVsbH0.w1liwK2gnZPOOaR3pxuqrJuWXRurfNwhACzqCZyMCFA&zhida_source=entity) 方法确保雨滴混合叠加后的 DistortTexture 能满足我们的需要。Lucas Bebber 的 [Rain & Water Effect Experiments](https://link.zhihu.com/?target=https%3A//tympanus.net/codrops/2015/11/04/rain-water-effect-experiments/) 实现中使用默认的 Alpha Blending，即

```glsl
Out.rgb = Src.a * Src.rgb + (1 - Src.a) * Dst.rgb // Blend SrcAlpha OneMinusSrcAlpha
```

但在实际测试中，这种混合方法得到的 DistortTexture 表现出堆叠的折射效果，难以表现连成一片的平滑液面。反复尝试后发发现 [Exclusion](https://link.zhihu.com/?target=https%3A//drafts.fxtf.org/compositing-1/%23blendingexclusion) 的混合模式恰好能够很好的满足需要。至于其[数学原理](https://zhida.zhihu.com/search?content_id=166711542&content_type=Article&match_order=1&q=%E6%95%B0%E5%AD%A6%E5%8E%9F%E7%90%86&zd_token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJ6aGlkYV9zZXJ2ZXIiLCJleHAiOjE3ODQ1NTM2MjUsInEiOiLmlbDlrabljp_nkIYiLCJ6aGlkYV9zb3VyY2UiOiJlbnRpdHkiLCJjb250ZW50X2lkIjoxNjY3MTE1NDIsImNvbnRlbnRfdHlwZSI6IkFydGljbGUiLCJtYXRjaF9vcmRlciI6MSwiemRfdG9rZW4iOm51bGx9.OyqK2SAXjuxm_eNkoV2I5b87J5aywGXkK1cmy-ck4TQ&zhida_source=entity)，本人才学疏浅，希望了解原理的评论区解释一下。

引入 Alpha 后的混合[表达式](https://zhida.zhihu.com/search?content_id=166711542&content_type=Article&match_order=1&q=%E8%A1%A8%E8%BE%BE%E5%BC%8F&zd_token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJ6aGlkYV9zZXJ2ZXIiLCJleHAiOjE3ODQ1NTM2MjUsInEiOiLooajovr7lvI8iLCJ6aGlkYV9zb3VyY2UiOiJlbnRpdHkiLCJjb250ZW50X2lkIjoxNjY3MTE1NDIsImNvbnRlbnRfdHlwZSI6IkFydGljbGUiLCJtYXRjaF9vcmRlciI6MSwiemRfdG9rZW4iOm51bGx9.FHbOzAcTXXdujTa12jo8LCEg1Z7374y7Uy54doRjEvY&zhida_source=entity)如下：

```glsl
Out.rgb = Src.a * Src.rgb + Dst.rgb - 2 * Src.a * Src.rgb * Dst.rgb
        = Src.a * Src.rgb * (1 - Dst.rgb) + Dst.rgb * (1 - Src.rgb)
```

即我们需要在 shader 中对 `fragColor` 预乘 `alpha` 之后对 RGB 采用 `OneMinusDstAlpha OneMinusSrcAlpha` 的混合模式，WebGL 和 Unity 中均可以实现。

![](v2-07.jpg)

上方一行为 normal 混合模式，下一行为 exclusion 混合模式

### Tiny Droplets

除了大颗的雨滴外，我们还可以随机渲染一些细小的水珠以提升画面效果，这些水珠的尺度大概在2-5像素，不会发生滑落，因此我们可以将这些水珠单独累积渲染到一张 RenderTexture 中，而滑落的雨滴将会擦除这些累积的水珠，这可以通过特殊的混合模式将上一部分得到的 raindropCompose Texture 的 alpha 通道渲染到水珠的 RenderTexture 中，对于 RGB 和 Alpha 通道均采用 `Zero OneMinusSrcAlpha` 的混合模式。

我们将这里渲染水珠并擦除得到的 Texture 和 raindropCompose Texture 采用 exclusion 模式混合后一并用于对背景图的折射采样，就得到下图这样的效果：

![](v2-08.jpg)

带有微小水珠的效果图

### Mist Rendering

我们还可以进一步加一层水雾效果，营造出屏幕内外的温差感（x

在我的实现中，对背景图采用 box filter 进行[降采样](https://zhida.zhihu.com/search?content_id=166711542&content_type=Article&match_order=1&q=%E9%99%8D%E9%87%87%E6%A0%B7&zd_token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJ6aGlkYV9zZXJ2ZXIiLCJleHAiOjE3ODQ1NTM2MjUsInEiOiLpmY3ph4fmoLciLCJ6aGlkYV9zb3VyY2UiOiJlbnRpdHkiLCJjb250ZW50X2lkIjoxNjY3MTE1NDIsImNvbnRlbnRfdHlwZSI6IkFydGljbGUiLCJtYXRjaF9vcmRlciI6MSwiemRfdG9rZW4iOm51bGx9.qZZequVWDl90BJkcdJtr_mah72Cmzc49BkpzaGVV2iQ&zhida_source=entity)+超采样模糊，以实现[景深效果](https://zhida.zhihu.com/search?content_id=166711542&content_type=Article&match_order=1&q=%E6%99%AF%E6%B7%B1%E6%95%88%E6%9E%9C&zd_token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJ6aGlkYV9zZXJ2ZXIiLCJleHAiOjE3ODQ1NTM2MjUsInEiOiLmma_mt7HmlYjmnpwiLCJ6aGlkYV9zb3VyY2UiOiJlbnRpdHkiLCJjb250ZW50X2lkIjoxNjY3MTE1NDIsImNvbnRlbnRfdHlwZSI6IkFydGljbGUiLCJtYXRjaF9vcmRlciI6MSwiemRfdG9rZW4iOm51bGx9.psDw9cNBah5a6SpsAholvUOjGBHqJmhz_2qaUHKk8UY&zhida_source=entity)。而水雾层需要更高的背景模糊程度作为覆盖，同时加上少量的[散射光](https://zhida.zhihu.com/search?content_id=166711542&content_type=Article&match_order=1&q=%E6%95%A3%E5%B0%84%E5%85%89&zd_token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJ6aGlkYV9zZXJ2ZXIiLCJleHAiOjE3ODQ1NTM2MjUsInEiOiLmlaPlsITlhYkiLCJ6aGlkYV9zb3VyY2UiOiJlbnRpdHkiLCJjb250ZW50X2lkIjoxNjY3MTE1NDIsImNvbnRlbnRfdHlwZSI6IkFydGljbGUiLCJtYXRjaF9vcmRlciI6MSwiemRfdG9rZW4iOm51bGx9.KUum8-aB4aqphQwcdhfZv-2fQzaaLUjU4ECqRzRJiwM&zhida_source=entity)效果。对水雾层同样采取前面提到的方式用雨滴进行擦除，以表现雨滴滑落流过的擦除效果。

![](./v2-09.jpg)

带水雾的效果

## Performance & Optimise

想必看到这大家应该都能发现两个明显的[性能瓶颈](https://zhida.zhihu.com/search?content_id=166711542&content_type=Article&match_order=1&q=%E6%80%A7%E8%83%BD%E7%93%B6%E9%A2%88&zd_token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJ6aGlkYV9zZXJ2ZXIiLCJleHAiOjE3ODQ1NTM2MjUsInEiOiLmgKfog73nk7bpoogiLCJ6aGlkYV9zb3VyY2UiOiJlbnRpdHkiLCJjb250ZW50X2lkIjoxNjY3MTE1NDIsImNvbnRlbnRfdHlwZSI6IkFydGljbGUiLCJtYXRjaF9vcmRlciI6MSwiemRfdG9rZW4iOm51bGx9.jJxGdKE5dZcs_vnVbUCKS1Qatucx80EixF-w0p_m2rU&zhida_source=entity)，雨滴的碰撞检测和渲染。朴素的[碰撞检测](https://zhida.zhihu.com/search?content_id=166711542&content_type=Article&match_order=2&q=%E7%A2%B0%E6%92%9E%E6%A3%80%E6%B5%8B&zd_token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJ6aGlkYV9zZXJ2ZXIiLCJleHAiOjE3ODQ1NTM2MjUsInEiOiLnorDmkp7mo4DmtYsiLCJ6aGlkYV9zb3VyY2UiOiJlbnRpdHkiLCJjb250ZW50X2lkIjoxNjY3MTE1NDIsImNvbnRlbnRfdHlwZSI6IkFydGljbGUiLCJtYXRjaF9vcmRlciI6MiwiemRfdG9rZW4iOm51bGx9.H4C0M4ittqf_lKadZOjx0xlMw7Z4M5DzQGw4QDHh_nM&zhida_source=entity)实现具有 O(N²) 的时间复杂度，而最简单的渲染对 N 个雨滴需要 N 次 DrawCall。

未优化的版本利用 Chrome DevTools 的性能测试结果如图，2000个雨滴，整个 update 包括模拟和渲染用了60ms……

![](./v2-10.jpg)

优化前的性能测试结果

### Grid-based Collision Check

对于碰撞检测的优化，简单地将屏幕区域[网格化](https://zhida.zhihu.com/search?content_id=166711542&content_type=Article&match_order=1&q=%E7%BD%91%E6%A0%BC%E5%8C%96&zd_token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJ6aGlkYV9zZXJ2ZXIiLCJleHAiOjE3ODQ1NTM2MjUsInEiOiLnvZHmoLzljJYiLCJ6aGlkYV9zb3VyY2UiOiJlbnRpdHkiLCJjb250ZW50X2lkIjoxNjY3MTE1NDIsImNvbnRlbnRfdHlwZSI6IkFydGljbGUiLCJtYXRjaF9vcmRlciI6MSwiemRfdG9rZW4iOm51bGx9.6HCCURd4Nd8uyKjoIpewbuKy03hDJZwZYQ2h0eC3uDk&zhida_source=entity)，网格尺寸取最大尺寸雨滴的融合距离的两倍，碰撞检测只需要对雨滴所在的区块和周围8个区块中的雨滴做测试。若60像素的网格尺寸，1080p的屏幕大约600个网格，2000个雨滴平均下来每个区块中只有3个雨滴，碰撞检测的时间复杂度大幅度降低。由于每个区块内维护的雨滴列表不需要有序，因此插入删除的时间复杂度为 O(1)（将列表末尾元素替换自身即可）优化后碰撞检测部分可以在2ms内完成。

### Draw Instancing

雨滴渲染的瓶颈可以利用 WebGL2 的 Instancing 解决，细小水珠的渲染则可以利用 `gl_InstanceID` 在 vertex shader 里计算随机 size 和 position 合成 model 矩阵进行[程序化](https://zhida.zhihu.com/search?content_id=166711542&content_type=Article&match_order=1&q=%E7%A8%8B%E5%BA%8F%E5%8C%96&zd_token=eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJ6aGlkYV9zZXJ2ZXIiLCJleHAiOjE3ODQ1NTM2MjUsInEiOiLnqIvluo_ljJYiLCJ6aGlkYV9zb3VyY2UiOiJlbnRpdHkiLCJjb250ZW50X2lkIjoxNjY3MTE1NDIsImNvbnRlbnRfdHlwZSI6IkFydGljbGUiLCJtYXRjaF9vcmRlciI6MSwiemRfdG9rZW4iOm51bGx9.BHcR0PQGqSPeRlWV5Xv6OoAoTZmMI_tb89fX-pWbETk&zhida_source=entity)的水珠渲染，随机数采用 [dcerisano 的 Gold Noise](https://link.zhihu.com/?target=https%3A//www.shadertoy.com/view/ltB3zD) 生成。

其他部分尽可能优化掉了所有可能创建临时 object 的代码，以降低开销

综合优化下来，2000个雨滴的性能开销降低到了 6ms 左右

![](./v2-11.jpg)

优化后的性能测试结果

移动端兼容性测试，Chrome 安卓，QQ 的内置浏览器都 OK，其他没有测试。在我 Mi 10 的 Chrome 安卓上性能测试跟 PC 无异，每帧update 在 7ms 左右。

代码放在了 GitHub：https://github.com/SardineFish/raindrop-fx

使用简单，但也具有大量可配置参数，详见 GitHub

```js
const canvas = document.queerySelector("#canvas");
const raindropFx = new RaindropFX({
    canvas: canvas,
    background: "url/to/backgroundImage",
});
raindropFx.start();
```