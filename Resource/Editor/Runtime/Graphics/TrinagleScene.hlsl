cbuffer CameraConstants : register(b0)
{
    float4x4 gView;
    float4x4 gProj;
};

struct VSInput
{
    float3 position : POSITION;
    float4 color : COLOR;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

PSInput VSMain(VSInput input)
{
    PSInput output;

    float4 pos = float4(input.position, 1.0f);

    float4 viewPos = mul(gView, pos);
    float4 projPos = mul(gProj, viewPos);

    output.position = projPos;
    output.color = input.color;

    return output;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return input.color;
}