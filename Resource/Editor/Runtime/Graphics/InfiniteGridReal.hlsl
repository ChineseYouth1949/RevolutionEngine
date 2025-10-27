cbuffer CameraConstants : register(b0) {
  float4x4 gView;
  float4x4 gProj;
};

struct VSInput {
  float2 position : POSITION;
};

struct PSInput {
  float4 svPosition : SV_POSITION;
  float2 texPosition : TEXCOORD0;
};

PSInput VSMain(VSInput input) {
  PSInput output;

  output.svPosition = float4(input.position, 0.0f, 1.0f);
  output.texPosition = input.position;

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
  float2 uvNDC = input.texPosition;
  float2 uvScreen = (input.texPosition + 1.0) / 2.0;

  float2 fragCoord = uvScreen * screenResolution;

  uvNDC.x *= screenResolution.x / screenResolution.y;

  float3 rayDir = normalize(float3(uvNDC, -1.5));
  float t = -cameraPos.y / rayDir.y;

  if (t < 0.0) { 
    return vec4(backgroundColor, 1.0); 
  }

  float3 worldPos = cameraPos + t * rayDir;

  float2 g = abs(fract(worldPos.xz / gridSize - 0.5) - 0.5) / fwidth(worldPos.xz);
  float gline = min(g.x, g.y);

  float mask = 1.0 - smoothstep(lineWidth - lineSoftness, lineWidth + lineSoftness, gline);
  
  // fade with distance (exponential dimming)
  float fade = exp(-0.02 * t);
  
  // composite grid over background
  float3 color = lerp(backgroundColor, gridColor, mask * fade);
  
  return vec4(color, 1.0); 
}