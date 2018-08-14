struct PSIn
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};

cbuffer cbPerFrame
{
    float3 mLightPosition;
};

Texture2D gColorMap;
Texture2D gNormalMap;
SamplerState gSamplerState;

float4 main(in PSIn _sIn) : SV_TARGET
{
    float3 normal = normalize(gNormalMap.Sample(gSamplerState, _sIn.TexCoord).xyz);
    float3 lightDir = normalize(mLightPosition);
    float light = saturate(dot(normal, lightDir));
    return gColorMap.Sample(gSamplerState, _sIn.TexCoord) * light;
}