struct PSIn
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

struct PSOut
{
    float4 Color : SV_TARGET0;
    float4 Normal : SV_TARGET1;
    float4 Material : SV_TARGET2;
};

Texture2D gColorMap;
SamplerState gSamplerState;

PSOut main(in PSIn _sIn)
{
    PSOut sOut;
    sOut.Color = gColorMap.Sample(gSamplerState, _sIn.TexCoord);
    sOut.Normal = float4(_sIn.Normal, 0.0);
    sOut.Material = float4(1.0, 1.0, 1.0, 1.0);
    return sOut;
}