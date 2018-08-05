#include <Game/Engine/ResourceHandler.hpp>

class Game : public ResourceHandler::Listener
{

public:

	Game ();

	void Tick ();

	void Destroy ();

	void SetWindow (GAME_NATIVE_WINDOW_T window, WindowSize size);

	void Size (WindowSize size);

	void Suspend ();

	void Resume ();

protected:

	void OnDeviceCreated () override;

	void OnDeviceDestroyed () override;

	void OnRender (double deltaTime) override;

	void OnSized (WindowSize size) override;

private:

	ResourceHandler m_ResourceHandler;

};