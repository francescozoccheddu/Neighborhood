struct PSIn
{
    float4 Position : SV_POSITION;
    float2 TexCoord : TEXCOORD;
};


Texture2D gColorMap;
Texture2D gNormalMap;
SamplerState gSamplerState;

float4 main(in PSIn _sIn) : SV_TARGET
{
    return gColorMap.Sample(gSamplerState, _sIn.TexCoord);
}