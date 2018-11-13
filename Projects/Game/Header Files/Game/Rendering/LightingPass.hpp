#pragma once

#include <Game/Rendering/RenderingPass.hpp>
#include <Game/Resources/ShaderResource.hpp>
#include <Game/Resources/ConstantBufferResource.hpp>
#include <Game/Rendering/ShadowingSubPass.hpp>
#include <Game/Resources/MeshResource.hpp>
#include <Game/Direct3D.hpp>
#include <Game/DirectXMath.hpp>
#include <Game/Scene/Scene.hpp>
#include <Game/Resources/SceneResources.hpp>


class LightingPass final : public RenderingPass
{

public:

	struct Inputs
	{
		const VertexShaderResource * screenShader;
		const ScreenMeshResource * mesh;
		ID3D11ShaderResourceView * normals;
		ID3D11ShaderResourceView * material;
		ID3D11ShaderResourceView * depth;
	};

	void Create (ID3D11Device & device) override final;

	void Destroy () override final;

	bool IsCreated () const override final;

	void Load () override final;

	void Unload () override final;

	bool IsLoaded () const override final;

	void Render (ID3D11DeviceContext & context, const SceneResources & sceneResources, const Scene & scene, const Inputs& inputs, ID3D11RenderTargetView * target);

private:

	static constexpr int s_cMaxLights { 32 };

	static std::list<const Light*> GatherLights (const Scene & scene);

	struct LightBufferData
	{
		struct PackedLight
		{
			DirectX::XMFLOAT3 position;
			UINT iShadowMap1Based;
			DirectX::XMFLOAT3 color;
			FLOAT intensity;
			DirectX::XMFLOAT3 direction;
			FLOAT innerCutoff;
			FLOAT linearAttenuation;
			FLOAT quadAttenuation;
			FLOAT cubicAttenuation;
			FLOAT outerCutoff;

			inline void Set (const Light & _light, UINT _iShadowMap1Based)
			{
				switch (_light.GetType ())
				{
					case Light::Type::POINT:
					{
						const PointLight & pointLight { static_cast<const PointLight&>(_light) };
						position = pointLight.position;
						direction = { 0.0f, 0.0f, 0.0f };
					}
					break;
					case Light::Type::DIRECTION:
					{

						const DirectionalLight & directionalLight { static_cast<const DirectionalLight&>(_light) };
						position = { 0.0f, 0.0f, 0.0f };
						direction = directionalLight.direction;
						innerCutoff = outerCutoff = 0.0f;
					}
					break;
					case Light::Type::CONE:
					{
						const ConeLight & coneLight { static_cast<const ConeLight&>(_light) };
						position = coneLight.position;
						direction = coneLight.direction;
					}
					break;
					default:
						GAME_THROW_MSG ("Unknown light type");
						break;
				}
				iShadowMap1Based = _iShadowMap1Based;
				color = _light.color;
				intensity = _light.intensity;
			}
		};

		alignas(16) UINT cLights;
		alignas(16) PackedLight aLights[s_cMaxLights];


		constexpr int GetSize () const
		{
			return GetSize (cLights);
		}

		static inline constexpr int GetSize (int _cLights)
		{
			return sizeof (PackedLight) * _cLights + offsetof (LightBufferData, aLights);
		}
	};

	struct TransformBufferData
	{
		DirectX::XMFLOAT4X4 invProjView;
		DirectX::XMFLOAT4X4 aTransforms[ShadowingSubPass::s_cConeMaps + ShadowingSubPass::s_cDirectionalMaps];

		static inline constexpr int GetSize (int _cLights)
		{
			return sizeof (DirectX::XMFLOAT4X4) * _cLights + offsetof (TransformBufferData, aTransforms);
		}
	};

	ShadowingSubPass m_ShadowingSubPass;

	PixelShaderResource m_PixelShader { GAME_PIXELSHADER_FILENAME ("Lighting") };

	ConstantBufferStructResource<TransformBufferData> m_TransformBuffer;
	ConstantBufferStructResource<LightBufferData> m_LightBuffer;

	com_ptr<ID3D11RasterizerState> m_RasterizerState;
	ID3D11SamplerState * m_apSamplerStates[2];

};