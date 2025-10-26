
cbuffer CameraConstants : register(b0) {
  float4x4 gView;
  float4x4 gProj;
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

  float4 viewPos = mul(gView, pos);
  float4 projPos = mul(gProj, viewPos);

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
  float3 worldPos = input.worldPos;

  // 计算网格线
  float2 gridCoord = worldPos.xz / gridSize;
  float2 gridLines = abs(frac(gridCoord - 0.5) - 0.5);

  // 应用抗锯齿（使用fwidth计算屏幕空间导数）
  float2 pixelSize = fwidth(gridLines);
  float gridLine = min(gridLines.x / pixelSize.x, gridLines.y / pixelSize.y);

  // 创建网格遮罩
  float mask = 1.0 - smoothstep(lineWidth - lineSoftness, lineWidth + lineSoftness, gridLine);

  // 计算基于距离的淡出效果
  float3 viewDir = worldPos - cameraPos;
  float distance = length(viewDir);
  float fade = exp(-0.02 * distance);

  // 混合颜色
  float3 color = lerp(backgroundColor.rgb, gridColor.rgb, mask * fade);

  return float4(color, 1.0);
}