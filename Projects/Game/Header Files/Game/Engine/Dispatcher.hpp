#include <Game/Engine/DeviceHolder.hpp>
#include <Game/Rendering/Renderer.hpp>
#include <Game/Utils/WindowRect.hpp>
#include <Game/Logic/Logic.hpp>

class Dispatcher
{

public:

	static void Initialize ();

	Dispatcher ();

	void Tick ();

	void Destroy ();

	void SetWindow (GAME_NATIVE_WINDOW_T window, WindowSize size, WindowRotation rotation);

	void Size (WindowSize size, WindowRotation rotation);

	void Suspend ();

	void Resume ();

	void ValidateDevice ();

	~Dispatcher ();

private:

	static double s_TimerFreq;
	bool m_bLastTimeValid { false };
	LARGE_INTEGER m_LastTime;
	DeviceHolder m_DeviceHolder;
	Renderer m_Renderer { m_DeviceHolder };
	Logic m_Logic;

	void Render ();

	void Update ();

};