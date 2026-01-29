cbuffer CameraCB : register(b0) {
    float4x4 view;
    float4x4 proj;
    float4x4 viewProj;
};

cbuffer ModelCB : register(b1) {
    float4x4 model;
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
    pos = mul(model, pos);
    pos = mul(viewProj, pos);

    result.pos = pos;
    result.color = input.color;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET {
    return input.color;
}
