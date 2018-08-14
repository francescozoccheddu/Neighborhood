#pragma once

#include <Game/Engine/DeviceHolder.hpp>
#include <Game/Scene/Scene.hpp>
#include <Game/Resources/SceneResources.hpp>
#include <Game/Resources/ShaderResource.hpp>
#include <Game/Resources/ConstantBufferResource.hpp>
#include <Game/Utils/WindowRect.hpp>
#include <Game/DirectXMath.hpp>
#include <Game/Direct3D.hpp>


class Renderer : public DeviceHolder::Listener
{

public:

	Renderer (const DeviceHolder & deviceHolder);

	void Render (const Scene& scene);

protected:

	void OnDeviceCreated () override;

	void OnDeviceDestroyed () override;

	void OnSized (WindowSize size, WindowRotation rotation) override;

private:

	enum
	{
		s_iColorRenderTarget,
		s_iNormalRenderTarget,
		s_iMaterialRenderTarget,
		s_cRenderTargets
	};

	static_assert(D3D11_SIMULTANEOUS_RENDER_TARGET_COUNT >= s_cRenderTargets);

	static constexpr DXGI_FORMAT s_renderTargetFormats[s_cRenderTargets] { DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_FORMAT_R8G8_UNORM };

	const DeviceHolder & m_DeviceHolder;
	com_ptr<ID3D11DepthStencilView> m_DepthStencilView;
	com_ptr<ID3D11RenderTargetView> m_RenderTargetViews[s_cRenderTargets];
	com_ptr<ID3D11ShaderResourceView> m_ShaderResourceViews[s_cRenderTargets];
	com_ptr<ID3D11SamplerState> m_SamplerState;
	D3D11_VIEWPORT m_Viewport;
	SceneResources m_SceneResources;
	ScreenMeshResource m_ScreenMesh;
	ShaderPassResource m_GeometryShaderPass;
	ShaderPassResource m_LightingShaderPass;

	struct cbGeometryPerFrame
	{
		DirectX::XMMATRIX projection;
		DirectX::XMMATRIX view;
	};

	ConstantBufferStructResource<cbGeometryPerFrame> m_constantBufferGeometry;

};