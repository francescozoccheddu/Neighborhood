struct PSIn
{
	float4 Position : SV_POSITION;
	float2 TexCoord : TEXCOORD;
};

struct Light
{
	float3 color;
	bool bCastShadow;
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

float4 GetWorldPosition (in float2 _texCoord)
{
	const float z = gDepthMap.Sample (gSamplerState, _texCoord).r;
	float4 screenPos = float4(_texCoord * float2(2, -2) + float2(-1, +1), z, 1.0);
	float4 worldPos = mul (screenPos, cb_InvProjView);
	return worldPos / worldPos.w;
	float4 worldPos2;
}

float GetShadowFactor (in float4 _position, in uint _iTransform, in uint _iMap)
{
	float4 projPos = mul (_position, cb_Transforms[_iTransform]);
	float2 shadowTexc = projPos.xy / projPos.w * float2(0.5, -0.5) + 0.5;
	float sampledDepth = gMaps2D[_iMap].SampleLevel (gSamplerState, shadowTexc, 0).r;
	float projDepth = projPos.z / projPos.w;
	return sampledDepth > projDepth ? 1.0 : 0.0;
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
	if (cb_Lights[iLight].bCastShadow)
	{
		shadowFactor = GetShadowFactor (position, iTransform, iLight);
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