#include <Game/Engine/Game.hpp>

Game::Game ()
{
	m_ResourceHandler.pListener = this;
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

void Game::OnDeviceCreated () {}

void Game::OnDeviceDestroyed () {}

void Game::OnRender (double deltaTime)
{
	float color[4] { 1.0f, 0.0f, 0.0f, 1.0f };
	m_ResourceHandler.GetDeviceContext ()->ClearRenderTargetView (m_ResourceHandler.GetRenderTargetView (), color);
}

void Game::OnSized (WindowSize size)
{}

