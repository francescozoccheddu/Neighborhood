cbuffer cbPerFrame
{
	float4x4 mProjection;
	float4x4 mView;
};

struct VOut
{
	float4 Pos : SV_POSITION;
	float3 Norm : COLOR;
};

VOut main (float3 inPos : POSITION, float3 inNorm : NORMAL)
{
	VOut v;
	v.Pos = mul (mul (float4(inPos, 1.0), mView), mProjection);
	v.Norm = inNorm;
	return v;
}