#pragma once

#include <Game/Engine/DeviceHolder.hpp>
#include <Game/Scene/Scene.hpp>
#include <Game/Resources/SceneResources.hpp>
#include <Game/Resources/ShaderResource.hpp>
#include <Game/Utils/WindowRect.hpp>

#define _GAME_RENDERER_TARGET_VIEWS_COUNT 3

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

	const DeviceHolder & m_DeviceHolder;
	com_ptr<ID3D11DepthStencilView> m_DepthStencilView;
	com_ptr<ID3D11RenderTargetView> m_RenderTargetViews[_GAME_RENDERER_TARGET_VIEWS_COUNT];
	com_ptr<ID3D11SamplerState> m_SamplerState;
	com_ptr<ID3D11Buffer> m_ConstantBufferPerFrame;
	D3D11_VIEWPORT m_Viewport;
	SceneResources m_SceneResources;
	ShaderPassResource m_GeometryShaderPass;

};