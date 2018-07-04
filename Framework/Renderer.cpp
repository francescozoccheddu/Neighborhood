#include "Renderer.hpp"
#include "View.hpp"

Renderer::Renderer (View & _view) : view{_view}
{}

Renderer::~Renderer ()
{}

void Renderer::Render (double _deltaTime)
{}

void Renderer::Show ()
{}

void Renderer::Hide ()
{}

void Renderer::Size (View::Size _size)
{}

void Renderer::Create ()
{}

void Renderer::Destroy ()
{}
