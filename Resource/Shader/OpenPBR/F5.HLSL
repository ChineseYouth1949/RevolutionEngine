// --- OpenPBR Ultimate Reference Pixel Shader ---

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
    float  spec_weight; float3 spec_color; float spec_rough; float spec_aniso; float spec_rot; float spec_ior;
    float  coat_weight; float3 coat_color; float coat_rough; float coat_aniso; float coat_rot; float coat_ior; float coat_dark;
    float  fuzz_weight; float3 fuzz_color; float fuzz_rough;
    float  emi_l; float3 emi_c;
    float  tf_w; float tf_t; float tf_ior;
    float  geometry_opacity;
};

cbuffer LightCB : register(b3) { float3 lightDir; float3 lightColor; };

static const float PI = 3.14159265f;

// --- 参考级物理库 ---

float sqr(float x) { return x * x; }

// 1. 各向异性 Alpha 映射 (修正版：遵循 OpenPBR 1.1 Eq. 21)
void GetAnisoAlphas(float r, float a, out float ax, out float ay) {
    float r2 = r * r;
    float oneMinusA = 1.0f - a;
    float denom = sqrt((1.0f + sqr(oneMinusA)) * 0.5f);
    ax = max(1e-4f, r2 / denom);
    ay = max(1e-4f, ax * oneMinusA);
}

// 2. 旋转切线空间
void RotateTangent(float3 T_raw, float3 B_raw, float rot, out float3 T, out float3 B) {
    float angle = rot * 2.0f * PI;
    float s, c; sincos(angle, s, c);
    T = c * T_raw + s * B_raw;
    B = -s * T_raw + c * B_raw;
}

// 3. 各向异性 GGX BRDF (D项 + V项)
float D_GGX_Aniso(float ax, float ay, float NoH, float ToH, float BoH) {
    float d = sqr(ToH / ax) + sqr(BoH / ay) + sqr(NoH);
    return 1.0f / (PI * ax * ay * sqr(d));
}

float V_SmithJoint_Aniso(float ax, float ay, float NoV, float NoL, float ToV, float ToL, float BoV, float BoL) {
    float visV = NoL * length(float3(ax * ToV, ay * BoV, NoV));
    float visL = NoV * length(float3(ax * ToL, ay * BoL, NoL));
    return 0.5f / max(visV + visL, 1e-5f);
}

// 4. Kulla-Conty 镜面反射能量补偿 (高粗糙度修正)
float3 GetSpecularEnergyCompensation(float NoV, float r, float3 F0) {
    float3 Favg = F0 + (1.0f - F0) / 21.0f;
    float E_NoV = 1.0f - r * 0.45f; 
    return Favg * (1.0f - E_NoV) / max(E_NoV, 1e-3f);
}

float4 PS_Main(PS_INPUT In) : SV_Target {
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

    // --- COAT LAYER ---
    float3 cT, cB; RotateTangent(T_raw, B_raw, coat_rot, cT, cB);
    float cax, cay; GetAnisoAlphas(coat_rough, coat_aniso, cax, cay);
    float F_coat = 0.04f + (1.0f - 0.04f) * pow(1.0f - VoH, 5.0f);
    float D_c = D_GGX_Aniso(cax, cay, NoH, dot(cT, H), dot(cB, H));
    float V_c = V_SmithJoint_Aniso(cax, cay, NoV, NoL, dot(cT, V), dot(cT, L), dot(cB, V), dot(cB, L));
    float3 specCoat = coat_weight * F_coat * D_c * V_c;

    // --- BASE SPECULAR ---
    // Eq. 60: IOR 耦合
    float eta_eff = lerp(spec_ior, spec_ior / coat_ior, coat_weight);
    float F0_d = sqr((eta_eff - 1.0f) / (eta_eff + 1.0f)) * spec_weight;
    // Eq. 61: 粗糙度卷积
    float rMod = lerp(spec_rough, pow(pow(spec_rough,4)+2*pow(coat_rough,4), 0.25f), coat_weight);
    float bax, bay; GetAnisoAlphas(rMod, spec_aniso, bax, bay);
    float3 bT, bB; RotateTangent(T_raw, B_raw, spec_rot, bT, bB);

    float3 F_base = lerp(float3(F0_d + (1.0f - F0_d) * pow(1.0f - VoH, 5.0f)), base_color * base_weight, base_metalness);
    
    // 薄膜虹光
    float3 tfTransmission = 1.0f;
    if (tf_w > 0) {
        float3 lambda = float3(650, 550, 450);
        float3 phase = (4.0f * PI * tf_ior * (tf_t * 1000.0f) * VoH) / lambda;
        float3 F_tf = F_base * (0.5f + 0.5f * cos(phase));
        tfTransmission = 1.0f - (F_tf * tf_w);
        F_base = lerp(F_base, F_tf, tf_w);
    }

    float D_b = D_GGX_Aniso(bax, bay, NoH, dot(bT, H), dot(bB, H));
    float V_b = V_SmithJoint_Aniso(bax, bay, NoV, NoL, dot(bT, V), dot(bT, L), dot(bB, V), dot(bB, L));
    float3 specBase = F_base * D_b * V_b + GetSpecularEnergyCompensation(NoV, rMod, F_base);

    // --- DIFFUSE LAYER (链式能量守恒) ---
    // 漫反射能量需扣除 Coat 和 BaseSpecular 的反射消耗
    float3 diffuseEnergyFactor = (1.0f - F_coat * coat_weight) * (1.0f - F_base) * tfTransmission;
    float3 diffBase = base_color * base_weight * (1.0f - base_metalness) / PI;
    
    // Eq. 65: 涂层变暗因子 Delta
    float Kr = 1.0f - (1.0f - 0.04f) / sqr(coat_ior);
    float3 Delta = (1.0f - Kr) / (1.0f - base_color * Kr);
    diffuseEnergyFactor *= lerp(1.0f, Delta * coat_color, coat_weight * coat_dark);

    // --- FINAL COMPOSITION ---
    float3 color = (specBase * (1.0f - F_coat * coat_weight)) + (diffBase * diffuseEnergyFactor) + specCoat;
    
    // Fuzz (Sheen)
    float fz = fuzz_weight * pow(1.0f - NoV, 4.0f);
    color = lerp(color, fuzz_color, fz);

    return float4(color * lightColor * NoL + emi_c * emi_l, geometry_opacity);
}