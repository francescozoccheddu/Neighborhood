#include <Game/Engine/Dispatcher.hpp>

#include <Game/Engine/GameListener.hpp>
#include <Game/Utils/Exceptions.hpp>
#include <Game/DirectXMath.hpp>
#include <Game/Engine/Renderer.hpp>

#define MAX_DELTA_TIME 1.0

void Dispatcher::Initialize ()
{
	if (!DirectX::XMVerifyCPUSupport ())
	{
		GAME_THROW_MSG ("DirectXMath not supported by this CPU");
	}
	LARGE_INTEGER timerFreq;
	if (!QueryPerformanceFrequency (&timerFreq))
	{
		GAME_THROW_MSG ("Timer frequency query failed");
	}
	s_TimerFreq = static_cast<double>(timerFreq.QuadPart);
}

double Dispatcher::s_TimerFreq = 0.0;


Dispatcher::Dispatcher ()
{
	m_DeviceHolder.pListener = &m_Renderer;
}

void Dispatcher::Tick ()
{
	Update ();
	Render ();
}

void Dispatcher::Destroy ()
{
	m_DeviceHolder.Destroy ();
}

void Dispatcher::SetWindow (GAME_NATIVE_WINDOW_T _pWindow, WindowSize _size, DXGI_MODE_ROTATION _rotation)
{
	m_DeviceHolder.SetWindow (_pWindow, _size, _rotation);
}

void Dispatcher::Size (WindowSize _size, DXGI_MODE_ROTATION _rotation)
{
	m_DeviceHolder.Size (_size, _rotation);
}

void Dispatcher::Suspend ()
{
	m_DeviceHolder.Trim ();
}

void Dispatcher::Resume ()
{
	m_bLastTimeValid = false;
}

void Dispatcher::ValidateDevice ()
{
	m_DeviceHolder.ValidateDevice ();
}

Dispatcher::~Dispatcher ()
{}

void Dispatcher::Render ()
{
	m_Renderer.Render (Scene {});
	m_DeviceHolder.Present ();
}

void Dispatcher::Update ()
{
	double deltaTime = 0.0;
	{
		LARGE_INTEGER newTime;
		if (!QueryPerformanceCounter (&newTime))
		{
			GAME_THROW_MSG ("Timer query failed");
		}
		if (!m_bLastTimeValid)
		{
			m_bLastTimeValid = true;
			deltaTime = 0.0;
		}
		else
		{
			deltaTime = static_cast<double>(newTime.QuadPart - m_LastTime.QuadPart) / s_TimerFreq;
			if (deltaTime < 0.0)
			{
				deltaTime = 0.0;
			}
			else if (deltaTime > MAX_DELTA_TIME)
			{
				deltaTime = MAX_DELTA_TIME;
			}
		}
		m_LastTime = newTime;
	}
}