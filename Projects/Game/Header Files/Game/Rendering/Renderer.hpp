#pragma once

#include <Game/Engine/DeviceHolder.hpp>
#include <Game/Scene/Scene.hpp>
#include <Game/Resources/ShaderResource.hpp>
#include <Game/Resources/DepthMapResource.hpp>
#include <Game/DirectXMath.hpp>
#include <Game/Direct3D.hpp>
#include <Game/Rendering/GeometryPass.hpp>
#include <Game/Rendering/LightingPass.hpp>

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

	static constexpr DXGI_FORMAT s_renderTargetFormats[s_cRenderTargets] { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_SNORM, DXGI_FORMAT_R32G32B32A32_FLOAT };

	const DeviceHolder & m_DeviceHolder;
	DepthMapResource * m_pDepthMapResource { nullptr };
	com_ptr<ID3D11RenderTargetView> m_RenderTargetViews[s_cRenderTargets];
	com_ptr<ID3D11ShaderResourceView> m_ShaderResourceViews[s_cRenderTargets];
	com_ptr<ID3D11SamplerState> m_SamplerState;
	D3D11_VIEWPORT m_Viewport;
	VertexShaderResource m_ScreenShader RENDERINGPASS_VERTSHADER ("Screen", ScreenMeshResource::s_aInputElementDesc);
	ScreenMeshResource m_ScreenMesh;
	GeometryPass m_GeometryPass;
	//LightingPass m_DirectionalLightingPass { m_GeometryPass };

};