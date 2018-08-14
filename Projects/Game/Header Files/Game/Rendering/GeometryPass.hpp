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

	void Create (ID3D11Device & device) override final;

	void Destroy () override final;

	bool IsCreated () const override final;

	void Load () override final;

	void Unload () override final;

	bool IsLoaded () const override final;

	void Render (const Scene & scene, ID3D11DeviceContext & context, const Target& target);

private:

	struct ConstantBuffer
	{
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMFLOAT4X4 view;
	};

	ShaderPassResource m_Shader RENDERINGPASS_SHADERPASS ("Geometry", SceneMeshResource);
	ConstantBufferStructResource<ConstantBuffer> m_ConstantBuffer;
	SceneResources m_SceneResources;

};