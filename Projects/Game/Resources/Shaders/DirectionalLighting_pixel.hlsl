struct PSIn
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

struct Light
{
    float3 direction;
    float3 color;
};

cbuffer cbPerFrame
{
    Light mLights[2];
};

Texture2D gNormalMap;
Texture2D gMaterialMap;
SamplerState gSamplerState;

float4 main(in PSIn _sIn) : SV_TARGET
{
    float3 normal = normalize(gNormalMap.Sample(gSamplerState, _sIn.TexCoord).xyz);
    float3 lightDir = normalize(mLights[0].direction);
    float light = saturate(dot(normal, lightDir));
    return float4(mLights[0].color * light, 1.0);
}