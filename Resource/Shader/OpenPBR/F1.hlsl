// --- OpenPBR Strict Pixel Shader (Highest Quality) ---
struct PS_INPUT {
    float4 Pos          : SV_POSITION; 
    float3 WorldPos     : TEXCOORD0;   
    float2 UV           : TEXCOORD1;   
    float3 WorldNormal  : TEXCOORD2;   
    float4 WorldTangent : TEXCOORD3;   
};

cbuffer CameraCB : register(b0) {
    float4x4 view; float4x4 proj; float4x4 viewProj; float3 cameraPos;
};

cbuffer OpenPBRParams : register(b2) {
    float  base_weight; float3 base_color; float base_metalness; float base_diffuse_roughness;
    float  specular_weight; float3 specular_color; float specular_roughness; float specular_roughness_anisotropy; float specular_ior;
    float  coat_weight; float3 coat_color; float coat_roughness; float coat_roughness_anisotropy; float coat_ior; float coat_darkening;
    float  fuzz_weight; float3 fuzz_color; float fuzz_roughness;
    float  emi_l; float3 emi_c;
    float  tf_w; float tf_t; float tf_ior;
    float  geometry_opacity;
};

cbuffer LightCB : register(b3) { float3 lightDir; float3 lightColor; };

static const float PI = 3.14159265f;

// --- 物理核心函数 ---

float sqr(float x) { return x * x; }

// Eq. 26: 物理 IOR 调制
float GetEffectiveIOR(float ior, float weight) {
    float Fs = sqr((ior - 1.0f) / (ior + 1.0f));
    float xi = clamp(weight, 0.0f, 1.0f / max(Fs, 1e-5f));
    float tmp = sign(ior - 1.0f) * sqrt(xi * Fs);
    return (1.0f + tmp) / max(1.0f - tmp, 1e-5f);
}

// Eq. 30: F82-Tint 金属 Fresnel
float3 F_F82(float cosTheta, float3 f0, float3 f82tint) {
    float Schlick = pow(1.0f - cosTheta, 5.0f);
    float3 Fs = f0 + (1.0f - f0) * Schlick;
    const float mu_bar = 1.0f / 7.0f;
    float denom = mu_bar * pow(1.0f - mu_bar, 6.0f);
    float3 F_bar = f0 + (1.0f - f0) * pow(1.0f - mu_bar, 5.0f);
    return Fs - cosTheta * pow(1.0f - cosTheta, 6.0f) * (1.0f - f82tint) * F_bar / denom;
}

// Eq. 36-39: EON 能量补偿漫反射 (Fujii 改进版)
float3 Evaluate_EON(float NoL, float NoV, float LoV, float3 albedo, float roughness) {
    float s = LoV - NoL * NoV;
    float stinv = (s > 0.0f) ? s / max(NoL, NoV) : 0.0f;
    float A = 1.0f / (1.0f + (0.5f - 2.0f / (3.0f * PI)) * roughness);
    float B = roughness * A;
    float3 f_ss = (albedo / PI) * (A + B * stinv);
    float avgE = A + (2.0f/3.0f - 64.0f/(45.0f*PI)) * B;
    float3 rho_ms = (albedo * albedo * avgE) / (1.0f - albedo * (1.0f - avgE));
    return f_ss + (rho_ms / PI) * (1.0f - avgE);
}

// Eq. 79: Zeltner Fuzz Albedo 近似
float FuzzAlbedo(float NoV, float r) {
    float s = r*(0.02066f + 1.58491f*r)/(0.03794f + r*(1.32227f + r));
    float m = r*(-0.19385f + r*(-1.14885f + r*(1.7932f - 0.95943f*r*r)))/(0.04639f + r);
    float o = r*(0.00065f + (-0.02078f + 0.11968f*r)*r)/(1.26264f + r*(-1.92021f + r));
    return exp(-0.5f * sqr((NoV - m)/s)) / (s * 2.5066f) + o; // 2.5066 = sqrt(2*PI)
}

// 各向异性映射 (Eq. 21)
void GetAnisoAlpha(float r, float a, out float ax, out float ay) {
    float r2 = r * r;
    float alpha = r2 * r2;
    ax = alpha / sqrt(max(1e-4f, 1.0f - a));
    ay = alpha * sqrt(max(1e-4f, 1.0f - a));
}

float D_GGX_Aniso(float ax, float ay, float XoH, float YoH, float NoH) {
    float d = sqr(XoH)/ax + sqr(YoH)/ay + sqr(NoH);
    return 1.0f / (PI * sqrt(ax * ay) * sqr(d));
}

float V_SmithJoint_Aniso(float ax, float ay, float NoV, float NoL, float XoV, float XoL, float YoV, float YoL) {
    float visV = NoL * sqrt(ax * sqr(XoV) + ay * sqr(YoV) + sqr(NoV));
    float visL = NoV * sqrt(ax * sqr(XoL) + ay * sqr(YoL) + sqr(NoL));
    return 0.5f / max(visV + visL, 1e-5f);
}

float4 PS_Main(PS_INPUT In) : SV_Target {
    float3 N = normalize(In.WorldNormal);
    float3 V = normalize(cameraPos - In.WorldPos);
    float3 L = normalize(lightDir);
    float3 H = normalize(V + L);
    float3 T = normalize(In.WorldTangent.xyz);
    float3 B = normalize(cross(N, T) * In.WorldTangent.w);

    float NoV = saturate(dot(N, V)) + 1e-5f;
    float NoL = saturate(dot(N, L)) + 1e-5f;
    float NoH = saturate(dot(N, H));
    float VoH = saturate(dot(V, H));

    // 1. Coat Roughening (Eq. 61)
    float rMod = lerp(specular_roughness, pow(pow(specular_roughness,4)+2*pow(coat_roughness,4), 0.25f), coat_weight);
    
    // 2. Base Layer (Metal vs Dielectric)
    float ax, ay; GetAnisoAlpha(rMod, specular_roughness_anisotropy, ax, ay);
    float n_eff = GetEffectiveIOR(specular_ior, specular_weight);
    float F0_d = sqr((n_eff - 1.0f) / (n_eff + 1.0f));
    float3 F_b = lerp(float3(F0_d + (1.0f - F0_d) * pow(1.0f - VoH, 5.0f)), 
                      F_F82(VoH, base_color * base_weight, specular_color), base_metalness);

    float3 specBase = F_b * D_GGX_Aniso(ax, ay, dot(T,H), dot(B,H), NoH) * V_SmithJoint_Aniso(ax, ay, NoV, NoL, dot(T,V), dot(T,L), dot(B,V), dot(B,L));
    float3 diffBase = Evaluate_EON(NoL, NoV, dot(L,V), base_color, base_diffuse_roughness) * (1.0f - base_metalness) * base_weight;

    // 3. Coat Darkening (Eq. 65)
    float Kr = 1.0f - (1.0f - 0.04f) / sqr(coat_ior); 
    float3 Delta = (1.0f - Kr) / (1.0f - base_color * Kr);
    float3 darkening = lerp(1.0f, Delta * coat_color, coat_weight * coat_darkening);

    // 4. Coat Specular
    float cax, cay; GetAnisoAlpha(coat_roughness, coat_roughness_anisotropy, cax, cay);
    float3 specCoat = D_GGX_Aniso(cax, cay, dot(T,H), dot(B,H), NoH) * V_SmithJoint_Aniso(cax, cay, NoV, NoL, dot(T,V), dot(T,L), dot(B,V), dot(B,L)) * coat_weight;

    // 5. Final Composition
    float F_c = 0.04f + (1.0f - 0.04f) * pow(1.0f - NoV, 5.0f);
    float3 color = (diffBase + specBase) * darkening;
    color = lerp(color, specCoat, coat_weight * F_c) * NoL;

    // Fuzz & Emission
    float fz = fuzz_weight * FuzzAlbedo(NoV, fuzz_roughness);
    color = lerp(color, fuzz_color * NoL, fz) + emi_c * emi_l;

    return float4(color * lightColor, geometry_opacity);
}