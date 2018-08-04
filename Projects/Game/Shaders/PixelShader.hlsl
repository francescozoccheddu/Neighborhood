struct PSIn
{
	float4 Pos : SV_POSITION;
	float4 Unused : TEXTCOORD;
	float4 Norm : COLOR;
};

float4 main (in PSIn v) : SV_TARGET
{
	return v.Norm;
}