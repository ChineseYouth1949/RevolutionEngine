cbuffer CameraCB : register(b0) {
    float4x4 view;
    float4x4 proj;
    float4x4 viewProj;
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

  float4 projPos = mul(viewProj, pos);

  output.position = projPos;
  output.worldPos = input.position;

  return output;
}

cbuffer GridConstants : register(b1) {
  float4 backgroundColor;
  float4 gridColor;
  float3 cameraPos;
  int2 screenResolution;
  float gridSize;
  float lineWidth;
  float lineSoftness;
};

float4 PSMain(PSInput input) : SV_TARGET {
      float2 coord = input.worldPos.xz / gridSize;
    
    // 1. 使用 fwidth 获取当前像素在网格坐标系下的变化率
    float2 derivative = fwidth(coord);
    
    // 2. 计算网格线逻辑：使用 fract 对齐并居中
    // 这能确保 grid 值在网格线处趋近于 0
    float2 grid = abs(frac(coord - 0.5) - 0.5) / derivative;
    
    // 3. 取 x 和 y 的最小值，即为到最近网格线的距离
    float lineDist = min(grid.x, grid.y);
    
    // 4. 关键：抗锯齿平滑处理
    // 这里的 1.0 代表 1 个像素宽度的抗锯齿过渡区
    float lineMask = 1.0 - smoothstep(0.0, 1.0, lineDist);

    // 5. 增强型距离衰减
    float3 viewDir = input.worldPos - cameraPos;
    float dist = length(viewDir);
    
    // 使用非线性衰减，让远方平滑消失，避免出现硬切边界
    float fade = exp(-0.01 * dist); 
    // 进一步限制：太远的地方直接不渲染网格，防止浮点数精度引起的闪烁
    fade *= saturate(1.0 - dist / 500.0); 

    // 6. 最终颜色混合
    // 注意：网格通常需要开启 Alpha Blend 以达到最佳效果
    float4 color = gridColor;
    color.a *= lineMask * fade;
    
    // 如果不开启 Blend，则使用 lerp
    float3 finalRGB = lerp(backgroundColor.rgb, gridColor.rgb, color.a);
    return float4(finalRGB, 1.0);
}