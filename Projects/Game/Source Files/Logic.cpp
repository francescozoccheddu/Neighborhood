#include <Game/Logic/Logic.hpp>
#include <Game/Engine/DeviceHolder.hpp>

Logic::Logic ()
{
	PerspectiveProjection* pProjection = new PerspectiveProjection;
	pProjection->vFov = 1.0f;
	pProjection->farZ = 10.0f;
	pProjection->nearZ = 0.05f;
	m_Scene.pProjection = pProjection;
	m_Scene.pView = new ViewWithOrientation ();
	m_Scene.pView->position = { 0.0f, 0.0f, -2.0f };


	Scene::Drawable sammy;
	sammy.pMesh = &m_MeshSammy;
	sammy.transform.Update ();
	m_Scene.drawables.push_back (sammy);

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

		/*{
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
		}*/

	}

	view.Update ();
}

const Scene& Logic::GetScene () const
{
	return m_Scene;
}

void Logic::OnDeviceDestroyed ()
{
	m_MeshSammy.Destroy ();
}

void Logic::OnDeviceCreated (const DeviceHolder& _deviceHolder)
{
	m_MeshSammy.Create (*_deviceHolder.GetDevice ());
}

void Logic::OnSized (const DeviceHolder& _deviceHolder)
{
	const WindowSize size { _deviceHolder.GetSize () };
	m_Scene.pProjection->aspectRatio = AbstractProjection::CalcAspectRatio (size.width, size.height);
	m_Scene.pProjection->Update ();
}