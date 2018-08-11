struct PSIn
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

Texture2D gColorMap;
SamplerState gSamplerState;

float4 main(in PSIn v) : SV_TARGET
{
    return gColorMap.Sample(gSamplerState, v.TexCoord);
}