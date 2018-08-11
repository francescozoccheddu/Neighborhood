cbuffer cbPerFrame
{
    float4x4 mProjection;
    float4x4 mView;
};

struct VSOut
{
    float4 Position : SV_POSITION;
    float3 Normal : NORMAL;
    float2 TexCoord : TEXCOORD;
};

VSOut main(in float3 inPosition : POSITION, in float3 inNormal : NORMAL, in float2 inTexCoord : TEXCOORD)
{
    VSOut v;
    v.Position = mul(mul(float4(inPosition, 1.0), mView), mProjection);
    v.Normal = inNormal;
    v.TexCoord = inTexCoord;
    return v;
}