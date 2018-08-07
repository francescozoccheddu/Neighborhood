#include <Game/Engine/ResourceHandler.hpp>
#include <Game/Direct3D.hpp>

class Game
{

public:

	static void Initialize ();

	Game ();

	void Tick ();

	void Destroy ();

	void SetWindow (GAME_NATIVE_WINDOW_T window, WindowSize size, DXGI_MODE_ROTATION rotation);

	void Size (WindowSize size, DXGI_MODE_ROTATION rotation);

	void Suspend ();

	void Resume ();

	void ValidateDevice ();

	~Game ();

private:

	ResourceHandler m_ResourceHandler;

};