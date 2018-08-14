#pragma once

#include <Game/Engine/DeviceHolder.hpp>
#include <Game/Scene/Scene.hpp>
#include <Game/Resources/ShaderResource.hpp>
#include <Game/DirectXMath.hpp>
#include <Game/Direct3D.hpp>
#include <Game/Rendering/GeometryPass.hpp>
#include <Game/Rendering/DirectionalLightingPass.hpp>

class Renderer : public DeviceHolder::Listener
{

public:

	friend class RenderingPass;

	Renderer (const DeviceHolder & deviceHolder);

	~Renderer ();

	void Render (const Scene& scene);

protected:

	void OnDeviceCreated () override;

	void OnDeviceDestroyed () override;

	void OnSized (WindowSize size, WindowRotation rotation) override;

private:

	enum
	{
		s_iColorTexture,
		s_iNormalTexture,
		s_iMaterialTexture,
		s_cRenderTargets
	};

	static_assert(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT >= s_cRenderTargets);

	static constexpr DXGI_FORMAT s_renderTargetFormats[s_cRenderTargets] { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_SNORM, DXGI_FORMAT_R8G8_UNORM };

	const DeviceHolder & m_DeviceHolder;
	com_ptr<ID3D11DepthStencilView> m_DepthStencilView;
	com_ptr<ID3D11RenderTargetView> m_RenderTargetViews[s_cRenderTargets];
	com_ptr<ID3D11ShaderResourceView> m_ShaderResourceViews[s_cRenderTargets];
	com_ptr<ID3D11SamplerState> m_SamplerState;
	D3D11_VIEWPORT m_Viewport;
	VertexShaderResource m_ScreenShader RENDERINGPASS_VERTSHADER ("Screen", ScreenMeshResource);
	ScreenMeshResource m_ScreenMesh;
	GeometryPass m_GeometryPass;
	DirectionalLightingPass m_DirectionalLightingPass;
	ConstantBufferStructResource<float> a;
	ConstantBufferStructResource<float> b;
	ConstantBufferStructResource<float> c;
	ConstantBufferStructResource<float> d;
	ConstantBufferStructResource<float> e;
	ConstantBufferStructResource<float> f;
	ConstantBufferStructResource<float> g;
	ConstantBufferStructResource<float> h;

};