cbuffer cbPerFrame
{
	float4x4 mProjView;
};

struct VSIn
{
	float3 Position : POSITION;
	float3 Normal : NORMAL;
	float3 Color : COLOR;
};

struct VSOut
{
	float4 Position : SV_POSITION;
	float3 Normal : NORMAL;
	float3 Color : COLOR;
	float3 WorldPosition : POSITION;
};

VSOut main (in VSIn _sIn)
{
	VSOut sOut;
	sOut.WorldPosition = _sIn.Position;
	sOut.Position = mul (float4(_sIn.Position, 1.0), mProjView);
	sOut.Normal = _sIn.Normal;
	sOut.Color = _sIn.Color;
	return sOut;
}