#pragma once

#include <string>
#include "ResourceHandler.hpp"
#include "Camera/View.hpp"
#include "Camera/Projection.hpp"
#include "Input/Gamepad.hpp"

class Game : public ResourceHandler
{

public:

	virtual void OnShow () override;

	virtual void OnHide () override;

	virtual void OnDeviceDestroyed () override;

	virtual void OnDeviceCreated () override;

	virtual void OnRender (double deltaTime) override;

	virtual void OnSized (WindowSize size) override;

private:

	Gamepad m_Gamepad;
	Camera::Projection m_CamProjection;
	Camera::ViewWithTarget m_CamView;
	UINT m_cInds;
	ID3D11Buffer * m_pVertexBuffer;
	ID3D11Buffer * m_pIndexBuffer;
	ID3D11Buffer * m_pConstantBuffer;
	ID3D11VertexShader * m_pVertexShader;
	ID3D11PixelShader * m_pPixelShader;
	ID3D11InputLayout * m_pInputLayout;

};

