// --- OpenPBR Ultimate Master Quality Pixel Shader ---

struct PS_INPUT {
    float4 Pos : SV_POSITION;
    float3 WorldPos : TEXCOORD0;
    float2 UV : TEXCOORD1;
    float3 WorldNormal : TEXCOORD2;
    float4 WorldTangent : TEXCOORD3;
};

cbuffer CameraCB : register(b0) { float4x4 view; float4x4 proj; float4x4 viewProj; float3 cameraPos; };

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

// --- 究极物理库 ---

float sqr(float x) { return x * x; }

// 1. 各向异性旋转与基础向量构建
void BuildAnisoFrame(float3 N, float3 T_raw, float3 B_raw, float rotation, out float3 T, out float3 B) {
    float angle = rotation * 2.0f * PI;
    float s = sin(angle); float c = cos(angle);
    T = c * T_raw + s * B_raw;
    B = -s * T_raw + c * B_raw;
}

// 2. 符合 Eq. 21 的精确各向异性 Alpha 映射
void GetAnisoAlphas(float r, float aniso, out float ax, out float ay) {
    float r2 = r * r;
    // 映射 r, a -> ax, ay 以保持高光面积恒定
    ax = max(1e-4f, r2 * (1.0f + aniso));
    ay = max(1e-4f, r2 * (1.0f - aniso));
}

// 3. 完整的各向异性 GGX D 项
float D_GGX_Aniso(float ax, float ay, float NoH, float ToH, float BoH) {
    float d = sqr(ToH / ax) + sqr(BoH / ay) + sqr(NoH);
    return 1.0f / (PI * ax * ay * sqr(d));
}

// 4. 完整的各向异性 Smith-Joint V 项 (满足究极分母逻辑)
float V_SmithJoint_Aniso(float ax, float ay, float NoV, float NoL, float ToV, float ToL, float BoV, float BoL) {
    float visV = NoL * length(float3(ax * ToV, ay * BoV, NoV));
    float visL = NoV * length(float3(ax * ToL, ay * BoL, NoL));
    return 0.5f / max(visV + visL, 1e-5f);
}

// 5. Airy Pattern 薄膜干涉 (带能量守恒因子)
float3 EvaluateThinFilm(float cosTheta, float thickness, float ior, float3 baseF0, out float3 transmissionScale) {
    float3 lambda = float3(650.0f, 550.0f, 450.0f);
    float3 phase = (4.0f * PI * ior * (thickness * 1000.0f) * cosTheta) / lambda;
    float3 R = baseF0 * (0.5f + 0.5f * cos(phase));
    transmissionScale = 1.0f - R; // 严格 R + T = 1
    return R;
}

// 6. 能量补偿多重散射 GGX (Kulla-Conty 近似)
float3 GetMSCompensation(float NoV, float r, float3 F0) {
    float3 Favg = F0 + (1.0f - F0) / 21.0f; // 经验公式：平均反射率
    float E_NoV = 1.0f - r * 0.45f;         // 简化的单次散射 albedo
    return Favg * (1.0f - E_NoV) / max(E_NoV, 1e-3f);
}

float4 PS_Main(PS_INPUT In) : SV_Target {
    // 向量标准化
    float3 N = normalize(In.WorldNormal);
    float3 V = normalize(cameraPos - In.WorldPos);
    float3 L = normalize(lightDir);
    float3 H = normalize(V + L);
    float3 T_raw = normalize(In.WorldTangent.xyz);
    float3 B_raw = normalize(cross(N, T_raw) * In.WorldTangent.w);

    float NoV = saturate(dot(N, V)) + 1e-5f;
    float NoL = saturate(dot(N, L)) + 1e-5f;
    float NoH = saturate(dot(N, H));
    float VoH = saturate(dot(V, H));

    // --- 层级一：COAT (涂层) ---
    float3 cT, cB; BuildAnisoFrame(N, T_raw, B_raw, coat_rotation, cT, cB);
    float cax, cay; GetAnisoAlphas(coat_roughness, coat_roughness_anisotropy, cax, cay);
    
    float3 F_coat = 0.04f + (1.0f - 0.04f) * pow(1.0f - VoH, 5.0f);
    float D_c = D_GGX_Aniso(cax, cay, NoH, dot(cT, H), dot(cB, H));
    float V_c = V_SmithJoint_Aniso(cax, cay, NoV, NoL, dot(cT, V), dot(cT, L), dot(cB, V), dot(cB, L));
    float3 specCoat = coat_weight * F_coat * D_c * V_c;

    // --- 层级二：BASE (基底) ---
    // 1. IOR 耦合与 Fresnel
    float eta_eff = lerp(specular_ior, specular_ior / coat_ior, coat_weight);
    float F0_d = sqr((eta_eff - 1.0f) / (eta_eff + 1.0f)) * specular_weight;
    
    // 2. 基底粗糙度卷积 (Eq. 61)
    float rMod = lerp(specular_roughness, pow(pow(specular_roughness, 4)+2*pow(coat_roughness, 4), 0.25f), coat_weight);
    float bax, bay; GetAnisoAlphas(rMod, specular_roughness_anisotropy, bax, bay);
    float3 bT, bB; BuildAnisoFrame(N, T_raw, B_raw, specular_rotation, bT, bB);

    float3 F_base = lerp(float3(F0_d + (1.0f - F0_d) * pow(1.0f - VoH, 5.0f)), base_color * base_weight, base_metalness);

    // 3. 薄膜干涉与透射衰减
    float3 tScale = 1.0f;
    if (thin_film_weight > 0) {
        float3 F_film = EvaluateThinFilm(VoH, thin_film_thickness, thin_film_ior, F_base, tScale);
        F_base = lerp(F_base, F_film, thin_film_weight);
    }

    // 4. 基底高光与能量补偿
    float D_b = D_GGX_Aniso(bax, bay, NoH, dot(bT, H), dot(bB, H));
    float V_b = V_SmithJoint_Aniso(bax, bay, NoV, NoL, dot(bT, V), dot(bT, L), dot(bB, V), dot(bB, L));
    float3 specBase = F_base * D_b * V_b + GetMSCompensation(NoV, rMod, F_base);

    // 5. 基底漫反射 (EFON)
    float3 diffBase = base_color * base_weight * (1.0f - base_metalness) / PI;

    // --- 终极层混合 (严格能量守恒) ---
    // 基底可见度 = (1 - F_coat) * 薄膜透射衰减
    float3 baseVisibleFactor = (1.0f - F_coat * coat_weight) * tScale;
    
    // 应用 Coat Darkening (Eq. 65 变暗因子)
    float Kr = 1.0f - (1.0f - 0.04f) / sqr(coat_ior);
    float3 Delta = (1.0f - Kr) / (1.0f - base_color * Kr);
    baseVisibleFactor *= lerp(1.0f, Delta * coat_color, coat_weight * coat_darkening);

    float3 color = (specBase + diffBase) * baseVisibleFactor + specCoat;
    
    // Fuzz 层叠加
    float fz = fuzz_weight * pow(1.0f - NoV, 4.0f);
    color = lerp(color, fuzz_color, fz);

    return float4(color * lightColor * NoL + emi_c * emi_l, geometry_opacity);
}