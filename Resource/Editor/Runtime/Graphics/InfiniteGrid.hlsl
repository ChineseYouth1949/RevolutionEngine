
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
  // 获取世界坐标
  float3 worldPos = input.worldPos;
  
  // 网格线计算（与原shader相同的算法）
  float2 gridCoord = worldPos.xz / gridSize;
  float2 fractional = abs(frac(gridCoord - 0.5) - 0.5);
  
  // 使用屏幕空间导数进行抗锯齿
  float2 pixelSize = fwidth(gridCoord);
  float2 gridDist = abs(fractional) / pixelSize;
  
  // 计算网格线遮罩
  float lineMask = 1.0 - smoothstep(lineWidth - lineSoftness, lineWidth + lineSoftness, min(gridDist.x, gridDist.y));
  
  // 计算距离衰减（基于相机到像素的距离）
  float3 viewDir = worldPos - cameraPos;
  float distance = length(viewDir);
  float fade = exp(-0.02 * distance);
  
  // 混合颜色
  float3 finalColor = lerp(backgroundColor.rgb, gridColor.rgb, lineMask * fade);
  
  return float4(finalColor, 1.0);
}