cbuffer cbPerFrame
{
    float4x4 mProjection;
    float4x4 mView;
};

struct VSOut
{
    float4 Pos : SV_POSITION;
    float3 Norm : NORMAL;
    float2 TexCoord : TEXCOORD;
};

VSOut main(in float3 inPos : POSITION, in float3 inNorm : NORMAL, in float2 inTexCoord : TEXCOORD)
{
    VSOut v;
    v.Pos = mul(mul(float4(inPos, 1.0), mView), mProjection);
    v.Norm = inNorm;
    v.TexCoord = inTexCoord;
    return
v;
}