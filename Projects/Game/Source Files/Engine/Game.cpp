#include <Game/Engine/Game.hpp>

#include <Game/Engine/GameListener.hpp>
#include <Game/Utils/Exceptions.hpp>
#include <Game/DirectXMath.hpp>

void Game::Initialize ()
{
	if (!DirectX::XMVerifyCPUSupport ())
	{
		GAME_THROW_MSG ("DirectXMath not supported by this CPU");
	}
	ResourceHandler::InitializeTimer ();
}

Game::Game ()
{
	m_ResourceHandler.pListener = new GameListener (m_ResourceHandler);
}

void Game::Tick ()
{
	m_ResourceHandler.Tick ();
}

void Game::Destroy ()
{
	m_ResourceHandler.Destroy ();
}

void Game::SetWindow (GAME_NATIVE_WINDOW_T _pWindow, WindowSize _size, DXGI_MODE_ROTATION _rotation)
{
	m_ResourceHandler.SetWindow (_pWindow, _size, _rotation);
}

void Game::Size (WindowSize _size, DXGI_MODE_ROTATION _rotation)
{
	m_ResourceHandler.Size (_size, _rotation);
}

void Game::Suspend ()
{
	m_ResourceHandler.Trim ();
}

void Game::Resume ()
{
	m_ResourceHandler.InvalidateTimer ();
}

void Game::ValidateDevice ()
{
	m_ResourceHandler.ValidateDevice ();
}

Game::~Game ()
{
	delete m_ResourceHandler.pListener;
	m_ResourceHandler.pListener = nullptr;
}

