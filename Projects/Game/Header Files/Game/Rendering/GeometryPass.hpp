#pragma once

#include <Game/Rendering/RenderingPass.hpp>
#include <Game/Resources/ShaderResource.hpp>
#include <Game/Resources/ConstantBufferResource.hpp>
#include <Game/Resources/MeshResource.hpp>
#include <Game/Direct3D.hpp>
#include <Game/DirectXMath.hpp>
#include <Game/Scene/Scene.hpp>
#include <Game/Resources/SceneResources.hpp>

class GeometryPass final : public RenderingPass
{

public:

	struct Target
	{
		ID3D11RenderTargetView * colors;
		ID3D11RenderTargetView * normals;
		ID3D11RenderTargetView * material;
		ID3D11DepthStencilView * depth;
	};

	struct ConstantBuffer
	{
		DirectX::XMFLOAT4X4 projView;
	};

	void Create (ID3D11Device & device) override final;

	void Destroy () override final;

	bool IsCreated () const override final;

	void Load () override final;

	void Unload () override final;

	bool IsLoaded () const override final;

	void Render (ID3D11DeviceContext & context, const SceneResources & sceneResources, const Scene & scene, const Target& target) const;

private:

	ShaderPassResource m_Shader RENDERINGPASS_SHADERPASS ("Geometry", SceneMeshResource::s_aInputElementDesc);
	mutable ConstantBufferStructResource<ConstantBuffer> m_ConstantBuffer;
	com_ptr<ID3D11SamplerState> m_SamplerState;
	com_ptr<ID3D11RasterizerState> m_RasterizerState;

};