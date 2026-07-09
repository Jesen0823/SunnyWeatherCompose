package org.jesen.dev.sunnyweather.pose.presentation.ui.components.widget

import androidx.compose.animation.core.*
import androidx.compose.foundation.Canvas
import androidx.compose.foundation.layout.*
import androidx.compose.material3.MaterialTheme
import androidx.compose.material3.Text
import androidx.compose.runtime.Composable
import androidx.compose.runtime.getValue
import androidx.compose.ui.Alignment
import androidx.compose.ui.Modifier
import androidx.compose.ui.geometry.Offset
import androidx.compose.ui.graphics.Color
import androidx.compose.ui.graphics.StrokeCap
import androidx.compose.ui.graphics.drawscope.Stroke
import androidx.compose.ui.platform.LocalDensity
import androidx.compose.ui.text.font.FontWeight
import androidx.compose.ui.text.style.TextOverflow
import androidx.compose.ui.unit.Dp
import androidx.compose.ui.unit.dp
import androidx.compose.ui.unit.sp
import org.jesen.dev.sunnyweather.pose.domain.model.Place
import kotlin.math.absoluteValue

/**
 * 页面指示器组件 - 串珠子样式
 * 
 * 设计说明：
 * - 文字在上，居中显示，无背景色，最多显示一行，过长截断
 * - 横向细线串联多个圆形珠子在下方，类似"算盘珠子"的视觉效果
 * - 当前选中的珠子尺寸变大（7dp），实心主题色
 * - 未选中的珠子使用默认大小（4.5dp），空心圆环灰色
 * - 采用分段画线法，线从圆边缘开始到下一个圆边缘结束，避免线穿透珠子
 * - 页面切换时有平滑的过渡动画效果
 * 
 * @param currentPage 当前页面索引
 * @param places 城市列表
 * @param modifier 修饰符
 * @param selectedColor 选中状态颜色（默认主题色）
 * @param unselectedColor 未选中状态颜色（默认灰色）
 * @param selectedSize 选中珠子大小（默认 7dp）
 * @param unselectedSize 未选中珠子大小（默认 4.5dp）
 * @param lineWidth 连接线宽度（默认 1.5dp）
 * @param itemSpacing 珠子中心间距（默认 24dp）
 */
@Composable
fun PagerIndicator(
    currentPage: Int,
    places: List<Place>,
    modifier: Modifier = Modifier,
    selectedColor: Color = MaterialTheme.colorScheme.primary,
    unselectedColor: Color = MaterialTheme.colorScheme.tertiary,
    selectedSize: Dp = 12.0.dp,
    unselectedSize: Dp = 8.0.dp,
    lineWidth: Dp = 3.0.dp,
    lineColor: Color = Color.Gray,
    itemSpacing: Dp = 24.dp,
    unselectedWidth:Dp=3.0.dp
) {
    if (places.isEmpty() || currentPage < 0 || currentPage >= places.size) {
        return
    }

    val animatedPage by animateFloatAsState(
        targetValue = currentPage.toFloat(),
        animationSpec = TweenSpec(
            durationMillis = 300,
            easing = FastOutSlowInEasing
        )
    )

    val density = LocalDensity.current
    val itemCount = places.size
    
    Column(
        modifier = modifier,
        horizontalAlignment = Alignment.CenterHorizontally,
        verticalArrangement = Arrangement.spacedBy(8.dp)
    ) {
        Text(
            text = places[currentPage].name,
            style = MaterialTheme.typography.labelSmall,
            fontWeight = FontWeight.Medium,
            color = Color.Black.copy(alpha = 0.7f),
            fontSize = 12.sp,
            maxLines = 1,
            overflow = TextOverflow.Ellipsis
        )

        if (itemCount > 1) {
            val itemSpacingPx = with(density) { itemSpacing.toPx() }
            val selectedRadius = with(density) { selectedSize.toPx() / 2f }
            val unselectedRadius = with(density) { unselectedSize.toPx() / 2f }
            val strokeWidth = with(density) { unselectedWidth.toPx() }
            val progressWidth = with(density) { lineWidth.toPx() }
            
            val canvasWidth = (itemCount - 1) * itemSpacingPx + selectedRadius * 2
            val canvasHeight = selectedSize

            Canvas(
                modifier = Modifier
                    .width(with(density) { canvasWidth.toDp() })
                    .height(canvasHeight)
            ) {
                val centerY = size.height / 2f
                val halfWidth = size.width / 2f

                val radii = mutableListOf<Float>()
                val centers = mutableListOf<Float>()

                places.forEachIndexed { index, _ ->
                    val progress = 1f - (animatedPage - index).absoluteValue.coerceAtMost(1f)
                    val radius = lerp(unselectedRadius, selectedRadius, progress)
                    val centerX = halfWidth - (itemCount - 1) * itemSpacingPx / 2f + index * itemSpacingPx
                    radii.add(radius)
                    centers.add(centerX)
                }

                for (i in 0 until itemCount - 1) {
                    val startX = centers[i] + radii[i]
                    val endX = centers[i + 1] - radii[i + 1]
                    
                    if (endX > startX) {
                        drawLine(
                            color = lineColor,
                            start = Offset(startX, centerY),
                            end = Offset(endX, centerY),
                            strokeWidth = progressWidth,
                            cap = StrokeCap.Round
                        )
                    }
                }

                places.forEachIndexed { index, _ ->
                    val centerX = centers[index]
                    val radius = radii[index]
                    val isSelected = index == currentPage
                    val progress = 1f - (animatedPage - index).absoluteValue.coerceAtMost(1f)
                    val alpha = lerp(0.4f, 1f, progress)

                    if (isSelected) {
                        drawCircle(
                            color = selectedColor.copy(alpha = alpha),
                            radius = radius,
                            center = Offset(centerX, centerY)
                        )
                    } else {
                        drawCircle(
                            color = unselectedColor,
                            radius = radius,
                            center = Offset(centerX, centerY),
                            style = Stroke(width = strokeWidth)
                        )
                    }
                }
            }
        }
    }
}

private fun lerp(start: Float, end: Float, fraction: Float): Float {
    return start + (end - start) * fraction
}
