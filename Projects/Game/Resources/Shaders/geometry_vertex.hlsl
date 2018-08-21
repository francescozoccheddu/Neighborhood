cbuffer cbPerFrame
{
    float4x4 mProjView;
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
    sOut.Position = mul(float4(_sIn.Position, 1.0), mProjView);
    sOut.Normal = _sIn.Normal;
    sOut.TexCoord = _sIn.TexCoord;
    return sOut;
}