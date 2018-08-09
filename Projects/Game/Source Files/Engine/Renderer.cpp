#include <Game/Engine/Renderer.hpp>

Renderer::Renderer (const DeviceHolder & _deviceHolder) : m_DeviceHolder { _deviceHolder } {}

void Renderer::OnDeviceCreated ()
{

}

void Renderer::OnDeviceDestroyed ()
{

}

void Renderer::OnSized (WindowSize _size, DXGI_MODE_ROTATION _rotation)
{

}

void Renderer::Render (const Scene & _scene)
{
	float color[4] { 0.0f, 1.0f, 0.0f, 1.0f };
	m_DeviceHolder.GetDeviceContext ()->ClearRenderTargetView (m_DeviceHolder.GetRenderTargetView (), color);
}