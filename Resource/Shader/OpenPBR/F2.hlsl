// --- OpenPBR Optimized Pixel Shader (High Performance) ---
struct PS_INPUT {
    float4 Pos : SV_POSITION; float3 WorldPos : TEXCOORD0; float2 UV : TEXCOORD1;
    float3 WorldNormal : TEXCOORD2; float4 WorldTangent : TEXCOORD3;
};

cbuffer CameraCB : register(b0) { float4x4 vp; float3 cameraPos; };
cbuffer OpenPBRParams : register(b2) {
    float bw; float3 bc; float bm; float bdr;
    float sw; float3 sc; float sr; float sa; float si;
    float cw; float3 cc; float cr; float ca; float ci; float cd;
    float fw; float3 fc; float fr;
    float el; float3 ec;
    float tfw; float tft; float tfi;
    float op;
};
cbuffer LightCB : register(b3) { float3 lDir; float3 lCol; };

float4 PS_Main(PS_INPUT In) : SV_Target {
    float3 N = normalize(In.WorldNormal);
    float3 V = normalize(cameraPos - In.WorldPos);
    float3 L = normalize(lDir);
    float3 H = normalize(V + L);

    float NoV = saturate(dot(N, V));
    float NoL = saturate(dot(N, L));
    float NoH = saturate(dot(N, H));
    float VoH = saturate(dot(V, H));

    // 1. 优化粗糙度 (线性替代 4次幂)
    float r = lerp(sr, sr + cr * 0.5f, cw);
    float a2 = sqr(r * r);

    // 2. 优化 Fresnel (Schlick 替代 F82)
    float f0 = sqr((si - 1.0f)/(si + 1.0f)) * sw;
    float3 F = lerp(f0, bc, bm) + (1.0f - lerp(f0, bc, bm)) * pow(1.0f - VoH, 5.0f);

    // 3. 极速 GGX (分离式 Smith)
    float D = a2 / (3.14159f * sqr(NoH * NoH * (a2 - 1.0f) + 1.0f));
    float G = NoL / (NoL * (1.0f - a2) + a2); // 快速 G

    float3 spec = F * D * G;
    float3 diff = bc * bw * (1.0f - bm) * 0.3183f;

    // 4. 优化变暗逻辑 (线性乘子替代 Delta)
    float3 base = (diff + spec) * lerp(1.0f, cc * 0.7f, cw * cd);
    
    // 5. 涂层简单叠加
    float cf = cw * (0.04f + 0.96f * pow(1.0f - NoV, 5.0f));
    float3 res = lerp(base * NoL, float3(D * 0.2f, D * 0.2f, D * 0.2f), cf);

    return float4(res * lCol + ec * el, op);
}