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

	struct DirectionalBuffer
	{
		alignas(16) DirectX::XMFLOAT4X4 invProjection;
		alignas(16) DirectX::XMFLOAT4X4 invView;
		alignas(16) UINT cLights;

		struct Light
		{
			alignas(16) DirectX::XMFLOAT4X4 transform;
			alignas(16) DirectX::XMFLOAT3 direction;
			alignas(16) DirectX::XMFLOAT3 color;
		} lights[D3D11_COMMONSHADER_INPUT_RESOURCE_REGISTER_COUNT];

		constexpr int GetSize () const
		{
			return GetSize (cLights);
		}

		static inline constexpr int GetSize (int _cLights)
		{
			return sizeof (Light) * _cLights + offsetof (DirectionalBuffer, lights);
		}
	};

	PixelShaderResource m_DirectionalShader RENDERINGPASS_PIXSHADER ("DirectionalLighting");
	ConstantBufferStructResource<DirectionalBuffer> m_DirectionalBuffer;
	ShadowingSubPass m_ShadowingSubPass;

};