#include <Game/Engine/ResourceHandler.hpp>

class Game : public ResourceHandler::Listener
{

public:

	Game ();

	void Tick ();

	void Destroy ();

	void SetWindow (GAME_NATIVE_WINDOW_T window, WindowSize size, DXGI_MODE_ROTATION rotation);

	void Size (WindowSize size, DXGI_MODE_ROTATION rotation);

	void Suspend ();

	void Resume ();

	void ValidateDevice ();

protected:

	void OnDeviceCreated () override;

	void OnDeviceDestroyed () override;

	void OnRender (double deltaTime) override;

	void OnSized (WindowSize size) override;

private:

	ResourceHandler m_ResourceHandler;

};