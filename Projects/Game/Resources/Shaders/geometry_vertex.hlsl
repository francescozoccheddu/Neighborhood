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
    float3 WorldPosition : POSITION;
};

VSOut main(in VSIn _sIn)
{
    VSOut sOut;
    sOut.WorldPosition = _sIn.Position;
    sOut.Position = mul(float4(_sIn.Position, 1.0), mProjView);
    sOut.Normal = _sIn.Normal;
    sOut.TexCoord = _sIn.TexCoord;
    return sOut;
}