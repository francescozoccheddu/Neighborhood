#pragma once

#include <Game/Engine/DeviceHolder.hpp>
#include <Game/Scene/Scene.hpp>
#include <Game/Resources/SceneResources.hpp>

#define _GAME_RENDERER_TARGET_VIEWS_COUNT 3

class Renderer : public DeviceHolder::Listener
{

public:

	Renderer (const DeviceHolder & deviceHolder);

	void Render (const Scene& scene);

protected:

	void OnDeviceCreated () override;

	void OnDeviceDestroyed () override;

	void OnSized (WindowSize size, DXGI_MODE_ROTATION rotation) override;

private:

	const DeviceHolder & m_DeviceHolder;
	com_ptr<ID3D11DepthStencilView> m_DepthStencilView;
	com_ptr<ID3D11RenderTargetView> m_RenderTargetViews[_GAME_RENDERER_TARGET_VIEWS_COUNT];
	D3D11_VIEWPORT m_Viewport;
	SceneResources m_SceneResources;
};