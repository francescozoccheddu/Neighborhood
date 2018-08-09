#pragma once

#include <Game/Direct3D.hpp>
#include <Game/Engine/ResourceHandler.hpp>
#include <Game/Camera/Projection.hpp>
#include <Game/Camera/View.hpp>
#include <Game/Input/Gamepad.hpp>
#include <Game/Resources/ResourceHolder.hpp>

class GameListener : public ResourceHandler::Listener
{

public:

	GameListener (ResourceHandler& resourceHandler);

	~GameListener () override = default;

	virtual void OnDeviceDestroyed () override;

	virtual void OnDeviceCreated () override;

	virtual void OnRender (double deltaTime) override;

	virtual void OnSized (WindowSize size, DXGI_MODE_ROTATION rotation) override;

private:

	ResourceHandler & m_ResourceHandler;
	Camera::Projection m_CamProjection;
	Camera::View m_CamView;
	Gamepad m_Gamepad;
	ResourceHolder m_ResourceHolder;
	ID3D11Buffer * m_pConstantBuffer { nullptr };
	ID3D11SamplerState * m_pSampler;

};

