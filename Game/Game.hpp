#pragma once

#include <string>
#include "View.hpp"
#include "ResourceHandler.hpp"

class Game : public ResourceHandler
{

public:

	Game (View& view);

	virtual void OnShow () override;

	virtual void OnHide () override;

	virtual void OnDeviceDestroyed () override;

	virtual void OnDeviceCreated () override;

	virtual void OnRender (double deltaTime) override;

	virtual void OnSized (View::Size size) override;

private:

	ID3D11Buffer * m_pBuffer;
	ID3D11VertexShader * m_pVertexShader;
	ID3D11PixelShader * m_pPixelShader;
	ID3D11InputLayout * m_pInputLayout;

};

