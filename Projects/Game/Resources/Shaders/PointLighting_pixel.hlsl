#define ENABLE_PCF 0

struct PSIn
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

struct Light
{
	float3 color;
	uint shadowMapSize;
	float3 position;
	float intensity;
	float3 radius;
	float farZ;
};

cbuffer cbLights : register(b0)
{
	float4x4 cb_InvProjView;
	uint cb_cLights;
	Light cb_Lights[32];
};

cbuffer cbTransforms : register(b1)
{
	float4x4 cb_Transforms[8];
};

Texture2D gDepthMap : register(t0);
Texture2D gNormalMap : register(t1);
Texture2D gMaterialMap : register(t2);
TextureCube gMaps[8] : register(t3);
SamplerState gSamplerState : register(s0);
SamplerComparisonState gSamplerComparisonState : register(s1);

float4 GetWorldPosition (in float2 _texCoord)
{
	const float z = gDepthMap.Sample (gSamplerState, _texCoord).r;
	float4 screenPos = float4(_texCoord * float2(2, -2) + float2(-1, +1), z, 1.0);
	float4 worldPos = mul (screenPos, cb_InvProjView);
	return worldPos / worldPos.w;
}

float VectorToDepthValue (const float3 Vec)
{
	float3 AbsVec = abs (Vec);
	float LocalZcomp = max (AbsVec.x, max (AbsVec.y, AbsVec.z));

	const float f = 10;
	const float n = 0.1;

	float NormZComp = -(f / (n - f) - (n * f) / (n - f) / LocalZcomp);

	return NormZComp;
}

float4 main (in PSIn _sIn) : SV_Target
{
	const float4 position = GetWorldPosition (_sIn.TexCoord);
const float3 normal = normalize (gNormalMap.Sample (gSamplerState, _sIn.TexCoord).xyz);
float3 light = float3(0.0, 0.0, 0.0);

const float3 lightVec = normalize (position.xyz - cb_Lights[0].position);
const float diffuseFactor = saturate (dot (normal, -lightVec));
const float attenuationFactor = saturate (1.0 / dot (lightVec, lightVec));

float shadowFactor = 1.0;

// BEGIN

float3 cubemapDir = position.xyz - cb_Lights[0].position;
float storedDepth = gMaps[0].Sample (gSamplerState, cubemapDir).r;

if (storedDepth < VectorToDepthValue (cubemapDir))
{
	shadowFactor = 0.0;
}


// END
const float finalLight = diffuseFactor * attenuationFactor * shadowFactor;

return float4(finalLight, finalLight, finalLight, 1.0);
}