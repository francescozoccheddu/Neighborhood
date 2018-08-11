#pragma once

#include <Game/Scene/Scene.hpp>
#include <Game/Input/Gamepad.hpp>
#include <Game/Utils/WindowRect.hpp>

class Logic
{

public:

	Logic ();

	void Update (double deltaTime);

	const Scene& GetScene () const;

	void Size (WindowSize size, WindowRotation rotation);

private:

	Scene m_Scene;
	Gamepad m_Gamepad;

};