#include <Game/Camera/View.hpp>

#include <DirectXMath.h>

using namespace DirectX;

#define FLYINGVIEW_BUTTON_RESET XINPUT_GAMEPAD_B
#define FLYINGVIEW_BUTTON_TURBO XINPUT_GAMEPAD_A
#define FLYINGVIEW_TRANSLATION_SPEED 1.0f
#define FLYINGVIEW_TRANSLATION_TURBO_SPEED 2.0f
#define FLYINGVIEW_ROTATION_SPEED 2.0f
#define FLYINGVIEW_RESET_POSITION XMVectorSet(0.0f,0.0f,-2.0f,1.0f)
#define FLYINGVIEW_RESET_UP XMVectorSet(0.0f,1.0f,0.0f,0.0f)
#define FLYINGVIEW_RESET_FORWARD XMVectorSet(0.0f,0.0f,1.0f,0.0f)
#define FLYINGVIEW_MAX_LOOKUP XMConvertToRadians(70.0f)
#define FLYINGVIEW_MIN_LOOKUP -XMConvertToRadians(70.0f)

namespace Camera
{

	CXMMATRIX AbstractView::Get () const
	{
		return m_mTransform;
	}

	ViewWithTarget::ViewWithTarget ()
	{
		ViewWithTarget::Update ();
	}

	void ViewWithTarget::Update ()
	{
		m_mTransform = XMMatrixTranspose (XMMatrixLookAtLH (position, target, unrotatedUp));
	}

	void View::Update ()
	{
		XMVECTOR quat { XMQuaternionRotationRollPitchYaw (lookUp, turn, tilt) };
		m_Up = XMVector3Rotate (unrotatedUp, quat);
		m_Forward = XMVector3Rotate (unrotatedForward, quat);
		m_Right = XMVector3Cross (m_Up, m_Forward);
		m_mTransform = XMMatrixTranspose (XMMatrixLookToLH (position, m_Forward, m_Up));
	}

	void View::Move (float _right, float _up, float _forward, float _speed)
	{
		position += (_right * m_Right + _up * m_Up + _forward * m_Forward) * _speed;
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

			const Gamepad::Thumb rThumb { _gamepad.GetThumb (Gamepad::Side::RIGHT, { 0.0f, 0.0f }) };
			const Gamepad::Thumb lThumb { _gamepad.GetThumb (Gamepad::Side::LEFT, { 0.0f, 0.0f }) };
			const float rTrig { _gamepad.GetTrigger (Gamepad::Side::RIGHT, 0.0f) };
			const float lTrig { _gamepad.GetTrigger (Gamepad::Side::LEFT, 0.0f) };

			Move (lThumb.x, lThumb.y, rTrig - lTrig, currTranslSpeed * _deltaTime);

			turn += rThumb.x * FLYINGVIEW_ROTATION_SPEED * _deltaTime;
			lookUp -= rThumb.y *  FLYINGVIEW_ROTATION_SPEED * _deltaTime;
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

	CXMVECTOR View::GetUp () const
	{
		return m_Up;
	}

	CXMVECTOR View::GetForward () const
	{
		return m_Forward;
	}

	CXMVECTOR View::GetRight () const
	{
		return m_Right;
	}

}
