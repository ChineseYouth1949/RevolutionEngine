cbuffer PerFrameConstants : register(b0) {
    float4x4 viewMatrix;
    float4x4 projMatrix;
};

cbuffer PerObjectConstants : register(b1) {
    float4x4 modelMatrix;
};

struct VSInput {
    float3 position : POSITION;
    float4 color : COLOR;
};

struct PSInput {
    float4 pos : SV_POSITION; 
    float4 color : COLOR;
};

PSInput VSMain(VSInput input) {
    PSInput result;

    float4 pos = float4(input.position, 1.0f);
    pos = mul(modelMatrix, pos);
    pos = mul(viewMatrix, pos);
    pos = mul(projMatrix, pos);

    result.pos = pos;
    result.color = input.color;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET {
    return input.color;
}
