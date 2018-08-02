cbuffer cbPerFrame
{
    float4x4 mProjection;
    float4x4 mView;
};

float4 main (float3 inPos : POSITION) : SV_POSITION
{
    return mul(mul(float4(inPos, 1.0), mView), mProjection);
}