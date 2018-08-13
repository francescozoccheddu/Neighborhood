cbuffer cbPerFrame
{
    float4x4 mProjection;
    float4x4 mView;
};

struct VSIn
{
    float3 Position : POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

struct VSOut
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

VSOut main(in VSIn _sIn)
{
    VSOut sOut;
    sOut.Position = mul(mul(float4(_sIn.Position, 1.0), mView), mProjection);
    sOut.Normal = _sIn.Normal;
    sOut.TexCoord = _sIn.TexCoord;
    return sOut;
}