struct MeshVertex
{
    float3 Position     : ATTRIBUTE0; 
    float3 Normal       : ATTRIBUTE1; 
    float4 Tangent      : ATTRIBUTE2;
    float2 UV0          : ATTRIBUTE3; 
};

struct VS_OUTPUT
{
    float4 Pos          : SV_POSITION; 
    float3 WorldPos     : TEXCOORD0;   
    float2 UV           : TEXCOORD1;   
    float3 WorldNormal  : TEXCOORD2;   
    float4 WorldTangent : TEXCOORD3;   
};

cbuffer CameraCB : register(b0)
{
    float4x4 view;
    float4x4 proj;
    float4x4 viewProj;
    float3   cameraPos;
};

struct PrimitiveData
{
    float4x4 localToWorld;
    float4x4 worldToLocal; 
};

StructuredBuffer<PrimitiveData> GpuSceneData : register(t0);

cbuffer RootConstants : register(b1)
{
    uint PrimitiveIndex;
};

#if defined(USE_INSTANCING)
struct InstanceData
{
    float4 Transform0   : ATTRIBUTE4; 
    float4 Transform1   : ATTRIBUTE5;
    float4 Transform2   : ATTRIBUTE6;
};
#endif

#if defined(USE_INSTANCING)
VS_OUTPUT VS_Main(MeshVertex In, InstanceData Instance)
#else
VS_OUTPUT VS_Main(MeshVertex In)
#endif
{
    VS_OUTPUT Out;

    float4x3 LocalToWorld;
    float3x3 NormalMatrix;

#if defined(USE_INSTANCING)
    LocalToWorld = float4x3(Instance.Transform0, Instance.Transform1, Instance.Transform2);
    NormalMatrix = (float3x3)LocalToWorld; 
#else
    LocalToWorld = (float4x3)GpuSceneData[PrimitiveIndex].localToWorld; 
    NormalMatrix = transpose((float3x3)GpuSceneData[PrimitiveIndex].worldToLocal);
#endif

    float3 WorldPos = mul(float4(In.Position, 1.0), LocalToWorld);
    Out.WorldPos = WorldPos;
    Out.Pos = mul(float4(WorldPos, 1.0), viewProj);
    Out.UV = In.UV0;

    float3 N = normalize(mul(In.Normal, NormalMatrix));
    float3 T = normalize(mul(In.Tangent.xyz, (float3x3)LocalToWorld));
    
    T = normalize(T - dot(T, N) * N);

    Out.WorldNormal = N;
    Out.WorldTangent = float4(T, In.Tangent.w);

    return Out;
}