struct PSIn
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

struct Light
{
    float4x4 transform;
    float3 direction;
    float3 color;
};

cbuffer cbPerFrame
{
    float4x4 cb_InvProj;
    float4x4 cb_InvView;
    uint cb_cLights;
    Light cb_Lights[128];
};

Texture2D gDepthMap : register(t0);
Texture2D gNormalMap : register(t1);
Texture2D gMaterialMap : register(t2);
Texture2D gMaps[4] : register(t3);
SamplerState gSamplerState;

float4 main(in PSIn _sIn) : SV_TARGET
{

    float3 normal = normalize(gNormalMap.Sample(gSamplerState, _sIn.TexCoord).xyz);
    float3 light = float3(0.0, 0.0, 0.0);
    const float4 worldPos = gMaterialMap.Sample(gSamplerState, _sIn.TexCoord);
    for (uint i = 0; i < cb_cLights; i++)
    {
        float3 lightDir = normalize(-cb_Lights[i].direction);
        float4 projPos = mul(worldPos, cb_Lights[i].transform);
        float2 shadowTexcoord = projPos.xy / projPos.w * float2(0.5, -0.5) + 0.5;
        float sampledDepth = gMaps[0].Sample(gSamplerState, shadowTexcoord).r;
        float projDepth = projPos.z / projPos.w;
        float shadowFactor = sampledDepth > projDepth ? 1.0 : 0.0;
        //light = saturate(light + cb_Lights[i].color * dot(normal, lightDir) * shadowFactor);
        light = float3(shadowFactor, shadowFactor, shadowFactor);
    }
    return float4(light, 1.0);
}