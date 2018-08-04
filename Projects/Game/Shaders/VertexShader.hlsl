cbuffer cbPerFrame
{
	float4x4 mProjection;
	float4x4 mView;
};

struct VOut
{
	float4 Pos : SV_POSITION;
	float4 Norm : COLOR;
	float4 Unused : TEXTCOORD;
};

VOut main (in float3 inPos : POSITION, in float3 inNorm : NORMAL)
{
	VOut v;
	v.Pos = mul (mul (float4(inPos, 1.0), mView), mProjection);
	v.Norm = float4(inNorm, 1.0);
	v.Unused = float4(0.0, 1.0, 0.0, 1.0);
	return v;
}