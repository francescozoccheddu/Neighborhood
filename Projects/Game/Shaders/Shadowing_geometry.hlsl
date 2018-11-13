#define MAX_ARRAY_SLICES 24

struct GSOut
{
	float4 Position : SV_Position;
	uint TargetSlice : SV_RenderTargetArrayIndex;
};

cbuffer cbPerFrame
{
	uint cb_SliceCount;
	float4x4 cb_Transforms[MAX_ARRAY_SLICES];
};

[maxvertexcount (MAX_ARRAY_SLICES * 3)]
void main (
	triangle float4 _sIn[3] : SV_Position,
	inout TriangleStream< GSOut > _sOut
)
{
	for (uint iSlice = 0; iSlice < cb_SliceCount; iSlice++)
	{
		for (uint iVert = 0; iVert < 3; iVert++)
		{
			GSOut element;
			element.Position = mul (_sIn[iVert], cb_Transforms[iSlice]);
			element.TargetSlice = iSlice;
			_sOut.Append (element);
		}
		_sOut.RestartStrip ();
	}
}