#include <Game/Camera/View.hpp>

#include <DirectXMath.h>

#define FLYINGVIEW_BUTTON_RESET XINPUT_GAMEPAD_B
#define FLYINGVIEW_BUTTON_TURBO XINPUT_GAMEPAD_A
#define FLYINGVIEW_TRANSLATION_SPEED 1.0f
#define FLYINGVIEW_TRANSLATION_TURBO_SPEED 2.0f
#define FLYINGVIEW_ROTATION_SPEED 2.0f
#define FLYINGVIEW_RESET_POSITION DirectX::XMVectorSet(0.0f,0.0f,-2.0f,1.0f)
#define FLYINGVIEW_RESET_UP DirectX::XMVectorSet(0.0f,1.0f,0.0f,0.0f)
#define FLYINGVIEW_RESET_FORWARD DirectX::XMVectorSet(0.0f,0.0f,1.0f,0.0f)
#define FLYINGVIEW_MAX_LOOKUP DirectX::XMConvertToRadians(70.0f)
#define FLYINGVIEW_MIN_LOOKUP -DirectX::XMConvertToRadians(70.0f)

namespace Camera
{

	DirectX::CXMMATRIX AbstractView::Get () const
	{
		return m_mTransform;
	}

	ViewWithTarget::ViewWithTarget ()
	{
		ViewWithTarget::Update ();
	}

	void ViewWithTarget::Update ()
	{
		m_mTransform = DirectX::XMMatrixTranspose (DirectX::XMMatrixLookAtLH (position, target, unrotatedUp));
	}

	void View::Update ()
	{
		DirectX::XMVECTOR quat{ DirectX::XMQuaternionRotationRollPitchYaw (lookUp, turn, tilt) };
		m_Up = DirectX::XMVector3Rotate (unrotatedUp, quat);
		m_Forward = DirectX::XMVector3Rotate (unrotatedForward, quat);
		m_Right = DirectX::XMVector3Cross (m_Up, m_Forward);
		m_mTransform = DirectX::XMMatrixTranspose (DirectX::XMMatrixLookToLH (position, m_Forward, m_Up));
	}

	void View::UpdateFlyingView (float _deltaTime, const Gamepad& _gamepad)
	{

		if (_gamepad.AreButtonsPressed (FLYINGVIEW_BUTTON_RESET, false))
		{
			position = FLYINGVIEW_RESET_POSITION;
			unrotatedUp = FLYINGVIEW_RESET_UP;
			unrotatedForward = FLYINGVIEW_RESET_FORWARD;
			tilt = lookUp = turn = 0.0f;
		}
		else
		{
			const float currTranslSpeed = _gamepad.AreButtonsPressed (FLYINGVIEW_BUTTON_TURBO, false) ? FLYINGVIEW_TRANSLATION_TURBO_SPEED : FLYINGVIEW_TRANSLATION_SPEED;

			const Gamepad::Thumb rThumb{ _gamepad.GetThumb (Gamepad::Side::RIGHT,{ 0.0f, 0.0f }) };
			const Gamepad::Thumb lThumb{ _gamepad.GetThumb (Gamepad::Side::LEFT,{ 0.0f, 0.0f }) };
			const float rTrig{ _gamepad.GetTrigger (Gamepad::Side::RIGHT, 0.0f) };
			const float lTrig{ _gamepad.GetTrigger (Gamepad::Side::LEFT, 0.0f) };

			DirectX::XMVECTOR velX{ DirectX::XMVectorScale (m_Right, lThumb.x) };
			DirectX::XMVECTOR velY{ DirectX::XMVectorScale (m_Up, lThumb.y) };
			DirectX::XMVECTOR velZ{ DirectX::XMVectorScale (m_Forward, rTrig - lTrig) };

			DirectX::XMVECTOR vel{ DirectX::XMVectorAdd (velX,DirectX::XMVectorAdd (velY,velZ)) };
			DirectX::XMVECTOR scaledVel{ DirectX::XMVectorScale (vel, _deltaTime * currTranslSpeed) };

			position = DirectX::XMVectorAdd (position, scaledVel);
			turn += rThumb.x * _deltaTime;
			lookUp -= rThumb.y * _deltaTime;
			if (lookUp > FLYINGVIEW_MAX_LOOKUP)
			{
				lookUp = FLYINGVIEW_MAX_LOOKUP;
			}
			else if (lookUp < FLYINGVIEW_MIN_LOOKUP)
			{
				lookUp = FLYINGVIEW_MIN_LOOKUP;
			}
		}

		Update ();
	}

	View::View ()
	{
		View::Update ();
	}

	DirectX::CXMVECTOR View::GetUp () const
	{
		return m_Up;
	}

	DirectX::CXMVECTOR View::GetForward () const
	{
		return m_Forward;
	}

	DirectX::CXMVECTOR View::GetRight () const
	{
		return m_Right;
	}

}
