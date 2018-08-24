#define ENABLE_PCF 1

struct PSIn
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

struct Light
{
	float3 color;
	uint shadowMapSize;
	float3 direction;
	float intensity;
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
Texture2D gMaps2D[8] : register(t3);
SamplerState gSamplerState : register(s0);
SamplerComparisonState gSamplerComparisonState : register(s1);

float4 GetWorldPosition (in float2 _texCoord)
{
	const float z = gDepthMap.Sample (gSamplerState, _texCoord).r;
	float4 screenPos = float4(_texCoord * float2(2, -2) + float2(-1, +1), z, 1.0);
	float4 worldPos = mul (screenPos, cb_InvProjView);
	return worldPos / worldPos.w;
	float4 worldPos2;
}

#if ENABLE_PCF
float GetShadowFactor (in float4 _position, in uint _iTransform, in uint _iMap, in float _pcfOffset)
#else
float GetShadowFactor (in float4 _position, in uint _iTransform, in uint _iMap)
#endif
{
	float4 projPos = mul (_position, cb_Transforms[_iTransform]);
	float2 shadowTexc = projPos.xy / projPos.w * float2(0.5, -0.5) + 0.5;
	float projDepth = projPos.z / projPos.w;

#if ENABLE_PCF
	float res = 0.0;
	const float2 offsets[9] = {
		float2(-1, -1), float2(0, -1), float2(1, -1),
		float2(-1, 0), float2(0, 0), float2(1, 0),
		float2(-1, +1), float2(0, +1), float2(1, +1)
	};

	for (uint iCmp = 0; iCmp < 9; iCmp++)
	{
		res += gMaps2D[_iMap].SampleCmpLevelZero (gSamplerComparisonState, shadowTexc + offsets[iCmp] * _pcfOffset, projDepth);
	}
	return res / 9.0f;
#else
	return gMaps2D[_iMap].SampleCmpLevelZero (gSamplerComparisonState, shadowTexc, projDepth);
#endif
}

float4 main (in PSIn _sIn) : SV_Target
{
	const float4 position = GetWorldPosition (_sIn.TexCoord);
const float3 normal = normalize (gNormalMap.Sample (gSamplerState, _sIn.TexCoord).xyz);
float3 light = float3(0.0, 0.0, 0.0);
uint iTransform = 0;
for (uint iLight = 0; iLight < cb_cLights; iLight++)
{
	float shadowFactor;
	if (cb_Lights[iLight].shadowMapSize)
	{
#if ENABLE_PCF
		shadowFactor = GetShadowFactor (position, iTransform, iLight, 1.0 / cb_Lights[iLight].shadowMapSize);
#else
		shadowFactor = GetShadowFactor (position, iTransform, iLight);
#endif
		iTransform++;
	}
	else
	{
		shadowFactor = 1.0;
	}
	float3 lightDir = normalize (-cb_Lights[iLight].direction);
	const float direct = dot (normal, lightDir);
	const float total = direct * shadowFactor;
	light = saturate (light + cb_Lights[iLight].color * total * cb_Lights[iLight].intensity);
}
return float4(light, 1.0);
}
