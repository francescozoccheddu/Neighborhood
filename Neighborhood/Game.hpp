#pragma once

#include <Framework/View.hpp>
#include <Framework/Renderer.hpp>

class Game : public Renderer
{

public:

	Game (View& view);

	virtual void OnShow () override;

	virtual void OnHide () override;

	virtual void OnDeviceDestroyed () override;

	virtual void OnDeviceCreated () override;

	virtual void OnRender (double deltaTime) override;

	virtual void OnSized (View::Size size) override;

};

