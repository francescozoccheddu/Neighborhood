struct PSIn
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float3 Color : COLOR;
	float3 WorldPosition : POSITION;
};

struct PSOut
{
	float4 Color : SV_TARGET0;
	float4 Normal : SV_TARGET1;
	float4 Material : SV_TARGET2;
};

PSOut main (in PSIn _sIn)
{
	PSOut sOut;
	sOut.Color = float4(_sIn.Color, 1.0);
	sOut.Normal = float4(_sIn.Normal, 0.0);
	sOut.Material = float4(_sIn.WorldPosition, 1.0);
	return sOut;
}