#include <Game/Engine/DeviceHolder.hpp>
#include <Game/Engine/Renderer.hpp>

class Dispatcher
{

public:

	static void Initialize ();

	Dispatcher ();

	void Tick ();

	void Destroy ();

	void SetWindow (GAME_NATIVE_WINDOW_T window, WindowSize size, DXGI_MODE_ROTATION rotation);

	void Size (WindowSize size, DXGI_MODE_ROTATION rotation);

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

	void Render ();

	void Update ();

};