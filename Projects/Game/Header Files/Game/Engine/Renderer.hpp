#pragma once

#include <Game/Engine/DeviceHolder.hpp>
#include <Game/Scene/Scene.hpp>

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

};