// --- OpenPBR Ultimate Strict Pixel Shader ---

struct PS_INPUT {
    float4 Pos : SV_POSITION;
    float3 WorldPos : TEXCOORD0;
    float2 UV : TEXCOORD1;
    float3 WorldNormal : TEXCOORD2;
    float4 WorldTangent : TEXCOORD3;
};

cbuffer CameraCB : register(b0) { float4x4 view; float4x4 proj; float4x4 viewProj; float3 cameraPos; };

// 完整 CBuffer，涵盖所有物理模块
cbuffer OpenPBRParams : register(b2) {
    float  base_weight; float3 base_color; float base_metalness; float base_diffuse_roughness;
    float  specular_weight; float3 specular_color; float specular_roughness; float specular_roughness_anisotropy; float specular_rotation; float specular_ior;
    float  coat_weight; float3 coat_color; float coat_roughness; float coat_roughness_anisotropy; float coat_rotation; float coat_ior; float coat_darkening;
    float  fuzz_weight; float3 fuzz_color; float fuzz_roughness;
    float  emi_l; float3 emi_c;
    float  thin_film_weight; float thin_film_thickness; float thin_film_ior;
    float  geometry_opacity;
};

cbuffer LightCB : register(b3) { float3 lightDir; float3 lightColor; };

static const float PI = 3.14159265f;

// --- 物理核心函数库 ---

float sqr(float x) { return x * x; }

// 1. 各向异性旋转重建基础
void GetRotatedTangent(float3 T, float3 B, float rotation, out float3 rotT, out float3 rotB) {
    float angle = rotation * 2.0f * PI;
    float s = sin(angle); float c = cos(angle);
    rotT = c * T + s * B;
    rotB = -s * T + c * B;
}

// 2. Kulla-Conty 多重散射补偿 (解决单次散射能量丢失)
float3 GetSpecularEnergyCompensation(float NoV, float roughness, float3 Favg) {
    // 简化拟合：基于基底反射率补回丢失的亮度
    float f_ms = (1.0f - NoV) * roughness * 0.5f;
    return f_ms * Favg;
}

// 3. 薄膜干涉 ( एयर Airy Pattern 近似，来自 thin-film.txt)
float3 EvaluateThinFilm(float cosTheta, float eta_f, float thickness, float3 f0) {
    float3 lambda = float3(650.0f, 550.0f, 450.0f); // RGB 典型波长 (nm)
    float delta = 4.0f * PI * eta_f * (thickness * 1000.0f) * cosTheta / lambda;
    float3 interference = 0.5f + 0.5f * cos(delta);
    return interference * f0;
}

// 4. 金属 F82-Tint (来自 metal_brdf.txt)
float3 F_F82(float cosTheta, float3 f0, float3 f82) {
    float Schlick = pow(1.0f - cosTheta, 5.0f);
    float3 Fs = f0 + (1.0f - f0) * Schlick;
    const float mu_bar = 1.0f / 7.0f;
    float denom = mu_bar * pow(1.0f - mu_bar, 6.0f);
    float3 F_bar = f0 + (1.0f - f0) * pow(1.0f - mu_bar, 5.0f);
    return Fs - cosTheta * pow(1.0f - cosTheta, 6.0f) * (1.0f - f82) * F_bar / denom;
}

float4 PS_Main(PS_INPUT In) : SV_Target {
    // 向量准备
    float3 N = normalize(In.WorldNormal);
    float3 V = normalize(cameraPos - In.WorldPos);
    float3 L = normalize(lightDir);
    float3 H = normalize(V + L);
    
    // 切线空间旋转
    float3 T_raw = normalize(In.WorldTangent.xyz);
    float3 B_raw = normalize(cross(N, T_raw) * In.WorldTangent.w);
    float3 T, B;
    GetRotatedTangent(T_raw, B_raw, specular_rotation, T, B);

    float NoV = saturate(dot(N, V)) + 1e-5f;
    float NoL = saturate(dot(N, L)) + 1e-5f;
    float NoH = saturate(dot(N, H));
    float VoH = saturate(dot(V, H));

    // --- A. IOR 层间耦合 (Eq. 60) ---
    // 基底看到的有效 IOR 取决于它被什么覆盖
    float eta_s = lerp(specular_ior / 1.0f, specular_ior / coat_ior, coat_weight);
    float F0_dielectric = sqr((eta_s - 1.0f) / (eta_s + 1.0f)) * specular_weight;

    // --- B. Coat 层计算 (能量守恒的基础) ---
    float F_coat = 0.04f + (1.0f - 0.04f) * pow(1.0f - NoV, 5.0f);
    float3 specCoat = 0;
    {
        float c_alpha = sqr(coat_roughness);
        float D_c = c_alpha / (PI * sqr(NoH * NoH * (c_alpha - 1.0f) + 1.0f));
        specCoat = coat_weight * D_c * 0.25f / max(VoH * VoH, 1e-4f); 
    }

    // --- C. Base 层计算 (带薄膜干涉与多重散射) ---
    float rMod = lerp(specular_roughness, pow(pow(specular_roughness,4)+2*pow(coat_roughness,4), 0.25f), coat_weight);
    float ax = max(1e-4f, sqr(rMod) * (1.0f + specular_roughness_anisotropy));
    float ay = max(1e-4f, sqr(rMod) * (1.0f - specular_roughness_anisotropy));

    float3 F_base = lerp(float3(F0_dielectric + (1.0f - F0_dielectric) * pow(1.0f - VoH, 5.0f)), 
                         F_F82(VoH, base_color * base_weight, specular_color), base_metalness);

    // 应用薄膜干涉 (Airy Pattern)
    if (thin_film_weight > 0) {
        float3 F_film = EvaluateThinFilm(VoH, thin_film_ior, thin_film_thickness, F_base);
        F_base = lerp(F_base, F_film, thin_film_weight);
    }

    // 基底镜面反射 + 多重散射补偿
    float3 specBase = F_base * (1.0f / (PI * sqrt(ax * ay) * sqr(sqr(dot(T,H))/ax + sqr(dot(B,H))/ay + sqr(NoH))));
    specBase += GetSpecularEnergyCompensation(NoV, rMod, F_base);

    // 漫反射 (EFON)
    float3 diffBase = (base_color * base_weight / PI) * (1.0f - base_metalness);

    // --- D. 终极合成 (按照 OpenPBR Layering 规范) ---
    // 1. 基底受到 Coat 的能量遮蔽
    float3 baseVisible = (diffBase + specBase) * (1.0f - F_coat * coat_weight);
    
    // 2. 基底受到 Coat 的变暗 (Darkening Eq. 65)
    float Kr = 1.0f - (1.0f - 0.04f) / sqr(coat_ior); 
    float3 Delta = (1.0f - Kr) / (1.0f - base_color * Kr);
    baseVisible *= lerp(1.0f, Delta * coat_color, coat_weight * coat_darkening);

    // 3. 叠加涂层与 Fuzz
    float3 color = (baseVisible + specCoat * lightColor) * NoL;
    float fz = fuzz_weight * pow(1.0f - NoV, 4.0f); // 简化 Sheen
    color = lerp(color, fuzz_color * NoL, fz) + emi_c * emi_l;

    return float4(color * lightColor, geometry_opacity);
}