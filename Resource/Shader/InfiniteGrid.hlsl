/* 
   优化版网格 Shader：解决远距离及大倾角摩尔纹
*/

cbuffer CameraCB : register(b0) {
    float4x4 view;
    float4x4 proj;
    float4x4 viewProj;
};

cbuffer GridConstants : register(b1) {
    float4 backgroundColor;
    float4 gridColor;
    float3 cameraPos;
    float padding; // 对齐
    int2 screenResolution;
    float gridSize;
    float lineWidth;     // 推荐设为 1.0 (像素单位)
    float lineSoftness;  // 推荐设为 1.0 (像素单位)
};

struct VSInput {
    float3 position : POSITION;
};

struct PSInput {
    float4 position : SV_POSITION;
    float3 worldPos : TEXCOORD0;
};

PSInput VSMain(VSInput input) {
    PSInput output;
    float4 pos = float4(input.position, 1.0f);
    output.position = mul(viewProj, pos);
    output.worldPos = input.position;
    return output;
}

float4 PSMain(PSInput input) : SV_TARGET {
    // 1. 准备坐标与偏导数
    float2 uv = input.worldPos.xz / gridSize;
    float2 dUV = fwidth(uv); // 获取当前像素跨越的 UV 范围
    
    // 2. 核心：计算平滑网格线
    // 使用 N-step 过滤：在 0-1 范围内寻找网格线
    // 将线宽从“世界单位”映射到“像素单位”
    float2 grid = abs(frac(uv - 0.5) - 0.5);
    
    // 计算线宽在 UV 空间的大小
    // lineWidth * dUV 表示线在当前像素下占据的 UV 厚度
    float2 targetWidth = lineWidth * dUV;
    
    // 使用 smoothstep 进行抗锯齿
    // 基于像素变化率 dUV 动态调整边缘模糊度
    float2 drawPos = smoothstep(targetWidth + lineSoftness * dUV, targetWidth, grid);
    
    // 取 X 和 Z 轴的最大值（合并网格线）
    float lineMask = max(drawPos.x, drawPos.y);

    // 3. 解决摩尔纹的关键：当网格太密时进行淡出 (Fading)
    // 如果单个像素已经跨越了将近半个网格周期(0.5)，则强制让网格消失
    float2 cellDensity = dUV; 
    float fadeOut = saturate(1.0 - max(cellDensity.x, cellDensity.y) * 2.0);

    // 4. 距离衰减 (距离相机太远时淡出)
    float3 viewDir = input.worldPos - cameraPos;
    float dist = length(viewDir);
    float distFade = exp(-0.005 * dist); // 调整参数控制远近
    distFade *= saturate(1.0 - dist / 800.0);

    // 5. 混合最终颜色
    // 推荐：在应用层开启 Alpha Blending (SrcAlpha, InvSrcAlpha)
    // 如果不开启，则按下方 lerp 处理
    float finalAlpha = lineMask * gridColor.a * fadeOut * distFade;
    
    float3 finalRGB = lerp(backgroundColor.rgb, gridColor.rgb, finalAlpha);
    
    return float4(finalRGB, 1.0);
}
