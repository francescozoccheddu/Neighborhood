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
    uint mCount;
    Light mLights[128];
};

Texture2D gNormalMap;
Texture2D gMaterialMap;
SamplerState gSamplerState;

float4 main(in PSIn _sIn) : SV_TARGET
{
    float3 normal = normalize(gNormalMap.Sample(gSamplerState, _sIn.TexCoord).xyz);
    float3 light = float3(0.0, 0.0, 0.0);
    for (uint i = 0; i < mCount; i++)
    {
        float3 lightDir = normalize(-mLights[i].direction);
        light = saturate(light + mLights[i].color * dot(normal, lightDir));
    }
    return float4(light, 1.0);
}