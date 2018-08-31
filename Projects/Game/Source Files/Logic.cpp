#include <Game/Logic/Logic.hpp>

Logic::Logic ()
{
	Scene::Drawable drawable;
	drawable.texture = "Sammy";
	drawable.mesh = "Sammy";
	drawable.transform.Update ();
	m_Scene.drawables.push_back (drawable);
	drawable.texture = "Home";
	drawable.mesh = "Home";
	drawable.transform.Update ();
	m_Scene.drawables.push_back (drawable);
	PerspectiveProjection* pProjection = new PerspectiveProjection;
	pProjection->vFov = 1.0f;
	pProjection->farZ = 10.0f;
	pProjection->nearZ = 0.05f;
	m_Scene.pProjection = pProjection;
	m_Scene.pView = new ViewWithOrientation ();
	m_Scene.pView->position = { 0.0f, 0.0f, -2.0f };
	DirectionalLight light;
	light.direction = { 0.5f, -0.75f, 1.0f };
	light.position = { -1.0f, 1.0f, -2.0f };
	light.color = { 0.2f, 0.2f, 1.0f };
	light.shadowNearZ = 0.5f;
	light.shadowFarZ = 10.0f;
	light.bCastShadows = true;
	light.shadowSize = 3.0f;
	light.Update ();
	//m_Scene.directionalLights.push_back (light);
	/*ConeLight light;
	light.direction = { 0.5f, -0.75f, 1.0f };
	light.position = { -1.0f, 1.0f, -2.0f };
	light.color = { 0.2f, 0.2f, 1.0f };
	light.shadowNearZ = 0.5f;
	light.shadowFarZ = 10.0f;
	light.innerAngle = 0.2f;
	light.outerAngle = 0.3f;
	light.bCastShadows = true;
	light.Update ();
	m_Scene.coneLight.push_back (light);*/
	PointLight light3;
	light3.position = { -1.0f, 1.0f, -2.0f };
	light3.color = { 0.2f, 0.2f, 1.0f };
	light3.shadowNearZ = 0.1f;
	light3.shadowFarZ = 10.0f;
	light3.bCastShadows = true;
	light3.Update ();
	m_Scene.pointLights.push_back (light3);
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

	ViewWithOrientation & view { *reinterpret_cast<ViewWithOrientation*>(m_Scene.pView) };

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

		{
			float x { 0 }, y { 0 }, z { 0 };
			if (m_Gamepad.AreButtonsPressed (XINPUT_GAMEPAD_DPAD_RIGHT, false))
				x += 1.0;
			if (m_Gamepad.AreButtonsPressed (XINPUT_GAMEPAD_DPAD_LEFT, false))
				x -= 1.0;
			if (m_Gamepad.AreButtonsPressed (XINPUT_GAMEPAD_DPAD_UP, false))
				z += 1.0;
			if (m_Gamepad.AreButtonsPressed (XINPUT_GAMEPAD_DPAD_DOWN, false))
				z -= 1.0;
			if (m_Gamepad.AreButtonsPressed (XINPUT_GAMEPAD_RIGHT_SHOULDER, false))
				y += 1.0;
			if (m_Gamepad.AreButtonsPressed (XINPUT_GAMEPAD_LEFT_SHOULDER, false))
				y -= 1.0;

			m_Scene.pointLights[0].position.x += x * fdt;
			m_Scene.pointLights[0].position.y += y * fdt;
			m_Scene.pointLights[0].position.z += z * fdt;
			m_Scene.pointLights[0].Update ();
		}

	}

	view.Update ();
}

const Scene& Logic::GetScene () const
{
	return m_Scene;
}

void Logic::Size (WindowSize _size, WindowRotation _rotation)
{
	m_Scene.pProjection->aspectRatio = AbstractProjection::CalcAspectRatio (_size.width, _size.height);
	m_Scene.pProjection->Update ();
}