struct PSIn
{
	float4 Pos : SV_POSITION;
	float4 Norm : NORMAL;
};

float4 main (in PSIn v) : SV_TARGET
{
	return v.Norm;
}