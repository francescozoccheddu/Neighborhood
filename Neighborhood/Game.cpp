#include "Game.hpp"
#include <Framework/View.hpp>
#include <Framework/Renderer.hpp>
#include <Framework/Direct3D11.h>

Game::Game (View & _view) : Renderer{_view}
{}

void Game::OnShow ()
{}

void Game::OnHide ()
{}

void Game::OnDeviceDestroyed ()
{}

void Game::OnDeviceCreated ()
{}

void Game::OnRender (double deltaTime)
{
	GetDeviceContext ()->ClearRenderTargetView (GetRenderTargetView (), DirectX::Colors::Aqua);
}

void Game::OnSized (View::Size size)
{}
