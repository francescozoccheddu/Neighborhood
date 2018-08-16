#include <Game/Logic/Logic.hpp>

Logic::Logic ()
{
	Scene::Drawable sammy;
	sammy.material.name = "Sammy";
	sammy.mesh = "Sammy";
	sammy.transform.Update ();
	m_Scene.drawables.push_back (sammy);
	m_Scene.projection.vFov = 1.0f;
	m_Scene.pView = new Scene::View ();
	m_Scene.pView->position = { 0.0f, 0.0f, -2.0f };
	Scene::DirectionalLight light;
	light.color = { 1.0f, 0.5f, 0.3f };
	light.direction = { 1.0f, 0.3f, .02f };
	m_Scene.directionalLights.push_back (light);
}

void Logic::Update (double _deltaTime)
{
	m_Gamepad.Update ();
	if (!m_Gamepad.IsConnected ())
	{
		DWORD iUser;
		if (Gamepad::FindNextController (0, iUser))
		{
			m_Gamepad.SetUserIndex (iUser);
			m_Gamepad.Update ();
		}
	}

	Scene::View & view { *reinterpret_cast<Scene::View*>(m_Scene.pView) };

	if (m_Gamepad.AreButtonsPressed (XINPUT_GAMEPAD_B, false))
	{
		view.position = { 0.0f, 0.0f, -2.0f };
		view.turn = view.lookUp = 0.0f;
	}
	else
	{
		Gamepad::Thumb lt { m_Gamepad.GetThumb (Gamepad::Side::LEFT, {}) };
		Gamepad::Thumb rt { m_Gamepad.GetThumb (Gamepad::Side::RIGHT, {}) };
		float fdt = static_cast<float>(_deltaTime);
		float speed = m_Gamepad.AreButtonsPressed (XINPUT_GAMEPAD_A, false) ? 2.f : 1.f;
		float angSpeed = DirectX::XMConvertToRadians (90);
		view.Move (lt.x, lt.y, m_Gamepad.GetTrigger (Gamepad::Side::RIGHT, 0.f) - m_Gamepad.GetTrigger (Gamepad::Side::LEFT, 0.f), speed * fdt);
		view.turn += rt.x * fdt * angSpeed;
		view.lookUp += -rt.y * fdt * angSpeed;
		view.ClampLookUp (DirectX::XMConvertToRadians (70.f));
	}

	view.Update ();
}

const Scene& Logic::GetScene () const
{
	return m_Scene;
}

void Logic::Size (WindowSize _size, WindowRotation _rotation)
{
	m_Scene.projection.aspectRatio = Scene::Projection::CalcAspectRatio (_size.width, _size.height);
	m_Scene.projection.Update ();
}