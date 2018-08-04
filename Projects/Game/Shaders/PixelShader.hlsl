float4 main (float4 Pos : SV_POSITION, float3 Norm : COLOR) : SV_TARGET
{
	return float4(Norm, 1.0);
}