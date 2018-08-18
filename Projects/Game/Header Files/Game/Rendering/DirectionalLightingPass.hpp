#pragma once

#include <Game/Rendering/RenderingPass.hpp>
#include <Game/Resources/ShaderResource.hpp>
#include <Game/Resources/ConstantBufferResource.hpp>
#include <Game/Resources/MeshResource.hpp>
#include <Game/Direct3D.hpp>
#include <Game/DirectXMath.hpp>
#include <Game/Scene/Scene.hpp>
#include <Game/Resources/SceneResources.hpp>

class DirectionalLightingPass final : public RenderingPass
{

public:

	struct Inputs
	{
		const VertexShaderResource * screenShader;
		const ScreenMeshResource * mesh;
		ID3D11ShaderResourceView * normals;
		ID3D11ShaderResourceView * material;
	};

	void Create (ID3D11Device & device) override final;

	void Destroy () override final;

	bool IsCreated () const override final;

	void Load () override final;

	void Unload () override final;

	bool IsLoaded () const override final;

	void Render (const std::vector<Scene::DirectionalLight> & lights, ID3D11DeviceContext & context, const Inputs& inputs, ID3D11RenderTargetView * target);

private:

	struct Light
	{
		DirectX::XMFLOAT3 direction;
		DirectX::XMFLOAT3 color;
	};

	struct ConstantBuffer
	{
		Light lights[2];
	};

	PixelShaderResource m_Shader RENDERINGPASS_PIXSHADER ("DirectionalLighting");
	ConstantBufferStructResource<ConstantBuffer> m_ConstantBuffer;

};