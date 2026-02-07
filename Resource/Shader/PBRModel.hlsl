struct VSIn {
  float3 position : POSITION;
  float3 normal : NORMAL;
  float3 tangent : TANGENT;
  float2 uv : TEXCOORD0;
};

struct VSOut {
  float4 pos : SV_POSITION;
  float3 worldPos : TEXCOORD0;
  float3 normal : TEXCOORD1;
  float3 tangent : TEXCOORD2;
  float2 uv : TEXCOORD3;
};

cbuffer CameraCB : register(b0) {
  float4x4 view;
  float4x4 proj;
  float4x4 viewProj;
};

cbuffer ModelCB : register(b1) {
  float4x4 model;
};

cbuffer MaterialCB : register(b2) {
  float4 albedo; // rgb + a
  float metallic;
  float roughness;
  float2 padding; // 显式对齐到 16 字节
};

VSOut VSMain(VSIn input) {
  VSOut o;
  float4 worldPos = mul(float4(input.position, 1.0f), model);
  o.worldPos = worldPos.xyz;
  o.pos = mul(worldPos, viewProj);
  float4 worldNormal = mul(float4(input.normal, 0.0f), model);
  o.normal = normalize(worldNormal.xyz);
  float4 worldTangent = mul(float4(input.tangent, 0.0f), model);
  o.tangent = normalize(worldTangent.xyz);
  o.uv = input.uv;
  return o;
}

// Simple PBR utilities (split-sum approximations omitted for brevity)
float3 fresnelSchlick(float cosTheta, float3 F0) {
  return F0 + (1 - F0) * pow(1 - cosTheta, 5);
}

float DistributionGGX(float3 N, float3 H, float roughness) {
  float a = roughness * roughness;
  float a2 = a * a;
  float NdotH = max(dot(N, H), 0.0);
  float NdotH2 = NdotH * NdotH;
  float denom = (NdotH2 * (a2 - 1.0) + 1.0);
  denom = 3.14159265359 * denom * denom;
  return a2 / denom;
}

float GeometrySchlickGGX(float NdotV, float roughness) {
  float r = (roughness + 1.0);
  float k = (r * r) / 8.0;
  return NdotV / (NdotV * (1.0 - k) + k);
}

float GeometrySmith(float3 N, float3 V, float3 L, float roughness) {
  float NdotV = max(dot(N, V), 0.0);
  float NdotL = max(dot(N, L), 0.0);
  float ggx1 = GeometrySchlickGGX(NdotV, roughness);
  float ggx2 = GeometrySchlickGGX(NdotL, roughness);
  return ggx1 * ggx2;
}

float4 PSMain(VSOut input) : SV_TARGET {
  float3 N = normalize(input.normal);
  float3 V = normalize(-input.worldPos); // assume camera at origin

  // Simple single directional light
  float3 L = normalize(float3(-0.5, -1.0, -0.3));
  float3 H = normalize(V + L);
  float3 lightColor = float3(1.0, 0.98, 0.9);
  float NdotL = max(dot(N, L), 0.0);

  float3 F0 = lerp(float3(0.04, 0.04, 0.04), albedo.rgb, metallic);
  float3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);
  float D = DistributionGGX(N, H, roughness);
  float G = GeometrySmith(N, V, L, roughness);

  float3 numerator = D * G * F;
  float denominator = 4.0 * max(dot(N, V), 0.001) * max(NdotL, 0.001) + 1e-6;
  float3 specular = numerator / denominator;

  float3 kS = F;
  float3 kD = (1.0 - kS) * (1.0 - metallic);

  float3 diffuse = albedo.rgb / 3.14159265359;

  float3 Lo = (kD * diffuse + specular) * lightColor * NdotL;

  // simple ambient
  float3 ambient = float3(0.03, 0.03, 0.03) * albedo.rgb;

  float3 color = ambient + Lo;
  color = color / (color + float3(1.0,1.0,1.0)); // tone mapping
  color = pow(color, float3(1.0/2.2)); // gamma

  return float4(color, albedo.a);
}
