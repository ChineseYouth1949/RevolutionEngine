// --- OpenPBR Optimized Pixel Shader (Master Speed) ---

float4 PS_Main(PS_INPUT In) : SV_Target {
    float3 N = normalize(In.WorldNormal);
    float3 V = normalize(cameraPos - In.WorldPos);
    float3 L = normalize(lightDir);
    float3 H = normalize(V + L);

    float NoV = saturate(dot(N, V));
    float NoL = saturate(dot(N, L));
    float VoH = saturate(dot(V, H));

    // 1. 线性简化：粗糙度与 IOR
    float r = lerp(spec_rough, spec_rough + coat_rough * 0.4f, coat_weight);
    float a2 = sqr(r * r);
    float f0 = sqr((spec_ior - 1.0f)/(spec_ior + 1.0f));

    // 2. 快速 Fresnel
    float3 F = lerp(f0, base_color, base_metalness);
    F += (1.0f - F) * pow(1.0f - VoH, 5.0f);

    // 3. 极速 GGX
    float D = a2 / (PI * sqr(dot(N, H) * dot(N, H) * (a2 - 1.0f) + 1.0f));
    float3 spec = F * D * 0.25f / max(VoH * VoH, 0.01f);

    // 4. 层级简化合成
    float cf = coat_weight * (0.04f + 0.96f * pow(1.0f - NoV, 5.0f));
    float3 base = (base_color * 0.318f + spec) * (1.0f - cf);
    float3 color = (base * NoL) + (spec * cf);

    return float4(color * lightColor + emi_c * emi_l, geometry_opacity);
}