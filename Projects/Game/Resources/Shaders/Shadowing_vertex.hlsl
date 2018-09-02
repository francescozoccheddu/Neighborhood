cbuffer cbPerModel : register(b0)
{
	float4x4 cb_ModelTransform;
};

float4 main (float3 _position : POSITION) : SV_Position
{
	return mul (float4 (_position, 1.0), cb_ModelTransform);
}