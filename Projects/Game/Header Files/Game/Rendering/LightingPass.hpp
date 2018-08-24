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

	LightingPass (const GeometryPass & geometryPass);

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

	void Render (const Scene & scene, ID3D11DeviceContext & context, const Inputs& inputs, ID3D11RenderTargetView * target);

private:

	static constexpr int s_cDirectionalTotal { 32 };
	static constexpr int s_cPointTotal { 32 };
	static constexpr int s_cConeTotal { 32 };
	static constexpr int s_cDirectionalWithShadow { 8 };
	static constexpr int s_cPointWithShadow { 4 };
	static constexpr int s_cConeWithShadow { 8 };
	static constexpr int s_cMaxWithShadow { std::max ({ s_cConeWithShadow, s_cDirectionalWithShadow, s_cPointWithShadow }) };

	struct DirectionalLightBufferData
	{
		DirectX::XMFLOAT3 color;
		UINT shadowMapSize;
		DirectX::XMFLOAT3 direction;
		FLOAT intensity;
	};

	struct ConeLightBufferData
	{
		DirectX::XMFLOAT3 color;
		UINT shadowMapSize;
		DirectX::XMFLOAT3 position;
		FLOAT intensity;
		DirectX::XMFLOAT3 direction;
		FLOAT innerCutoff;
		FLOAT outerCutoff;
		FLOAT startLength;
		FLOAT endLenght;
		FLOAT realEndLength;
	};

	struct PointLightBufferData
	{
		DirectX::XMFLOAT3 color;
		UINT shadowMapSize;
		DirectX::XMFLOAT3 position;
		FLOAT intensity;
		DirectX::XMFLOAT3 radius;
	};

	template<typename T, int cMax = 1>
	struct LightBufferData
	{
		DirectX::XMFLOAT4X4 invProjView;
		alignas(16) UINT cLights;

		alignas(16) T lights[cMax];


		constexpr int GetSize () const
		{
			return GetSize (cLights);
		}

		static inline constexpr int GetSize (int _cLights)
		{
			return static_cast<int>(sizeof (T)) * _cLights + 16 * 5;
		}
	};

	struct TransformBufferData
	{
		DirectX::XMFLOAT4X4 transforms[s_cMaxWithShadow];

		static inline constexpr int GetSize (int _cLights)
		{
			return static_cast<int>(sizeof (DirectX::XMFLOAT4X4)) * _cLights;
		}
	};

	PixelShaderResource m_DirectionalShader RENDERINGPASS_PIXSHADER ("DirectionalLighting");
	PixelShaderResource m_ConeShader RENDERINGPASS_PIXSHADER ("ConeLighting");
	PixelShaderResource m_PointShader RENDERINGPASS_PIXSHADER ("PointLighting");
	ConstantBufferResource m_LightBuffer { std::max ({ LightBufferData<DirectionalLightBufferData>::GetSize (s_cDirectionalTotal) }) };
	ConstantBufferStructResource<TransformBufferData> m_TransformBuffer;
	ShadowingSubPass m_ShadowingSubPass;
	LightBufferData<DirectionalLightBufferData, s_cDirectionalTotal> m_DirectionalBufferData;
	LightBufferData<ConeLightBufferData, s_cConeTotal> m_ConeBufferData;
	LightBufferData<PointLightBufferData, s_cPointTotal> m_PointBufferData;
	com_ptr<ID3D11RasterizerState> m_RasterizerState;
	com_ptr<ID3D11SamplerState> m_SamplerState;
	com_ptr<ID3D11SamplerState> m_SamplerComparisonState;

};