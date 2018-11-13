#define MAX_LIGHTS 32

cbuffer cbTransforms
{
	float4x4 cb_InvProjView;
	float4x4 cb_aTransforms[MAX_LIGHTS];
};

struct Light
{
	float3 Position;
	uint iMap1Based;
	float3 Color;
	float Intensity;
	float3 Direction;
	float InnerCutoff;
	float3 Attenuation;
	float OuterCutoff;
};

cbuffer cbLights
{
	uint cb_cLights;
	Light cb_aLights[MAX_LIGHTS];
};

struct PSIn
{
	float4 Position : SV_Position;
	float2 TexCoord : TEXCOORD;
};

Texture2DArray g_taConeMaps;
Texture2DArray g_taDirectionalMaps;
TextureCubeArray g_taCPointMaps;

Texture2D g_tDepth;
Texture2D g_tNormal;
Texture2D g_tMaterial;

SamplerState g_sGBuffer;
SamplerComparisonState g_sComparison;

float4 main (in PSIn _sIn) : SV_Target {
	return 1.0;
}