#pragma once

#include <string>
#include <Game/Engine/ResourceHandler.hpp>
#include <Game/Camera/View.hpp>
#include <Game/Camera/Projection.hpp>

class GameListener : public ResourceHandler::Listener
{

public:

	GameListener (ResourceHandler& resourceHandler);

	~GameListener () override = default;

	virtual void OnDeviceDestroyed () override;

	virtual void OnDeviceCreated () override;

	virtual void OnRender (float deltaTime) override;

	virtual void OnSized (WindowSize size) override;

private:

	ResourceHandler & m_ResourceHandler;
	Camera::Projection m_CamProjection;
	Camera::View m_CamView;
	UINT m_cInds;
	ID3D11Buffer * m_pVertexBuffer { nullptr };
	ID3D11Buffer * m_pIndexBuffer { nullptr };
	ID3D11Buffer * m_pConstantBuffer { nullptr };
	ID3D11VertexShader * m_pVertexShader { nullptr };
	ID3D11PixelShader * m_pPixelShader { nullptr };
	ID3D11InputLayout * m_pInputLayout { nullptr };

};

