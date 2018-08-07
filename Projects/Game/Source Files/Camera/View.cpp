#include <Game/Camera/View.hpp>

using namespace DirectX;

namespace Camera
{

	XMMATRIX AbstractView::Get () const
	{
		return XMLoadFloat4x4 (&m_mTransform);
	}

	ViewWithTarget::ViewWithTarget ()
	{
		ViewWithTarget::Update ();
	}

	void ViewWithTarget::Update ()
	{
		CXMMATRIX transform = XMMatrixTranspose (XMMatrixLookAtLH (XMLoadFloat3 (&position), XMLoadFloat3 (&target), XMLoadFloat3 (&unrotatedUp)));
		XMStoreFloat4x4 (&m_mTransform, transform);
	}

	void View::Update ()
	{
		CXMVECTOR quat { XMQuaternionRotationRollPitchYaw (lookUp, turn, tilt) };
		CXMVECTOR up { XMVector3Rotate (XMLoadFloat3 (&unrotatedUp), quat) };
		CXMVECTOR forward { XMVector3Rotate (XMLoadFloat3 (&unrotatedForward), quat) };
		CXMVECTOR right { XMVector3Cross (up, forward) };
		CXMMATRIX transform = XMMatrixTranspose (XMMatrixLookToLH (XMLoadFloat3 (&position), forward, up));
		XMStoreFloat3 (&m_Up, up);
		XMStoreFloat3 (&m_Forward, forward);
		XMStoreFloat3 (&m_Right, right);
		XMStoreFloat4x4 (&m_mTransform, transform);
	}

	void View::Move (float _right, float _up, float _forward, float _speed)
	{
		CXMVECTOR newPos = (_right * XMLoadFloat3 (&m_Right) + _up * XMLoadFloat3 (&m_Up) + _forward * XMLoadFloat3 (&m_Forward)) * _speed;
		XMStoreFloat3 (&position, newPos);
	}

	void View::ClampLookUp (float _angle)
	{
		if (lookUp > _angle)
		{
			lookUp = _angle;
		}
		else if (lookUp < _angle)
		{
			lookUp = _angle;
		}
	}

	View::View ()
	{
		View::Update ();
	}

	XMVECTOR View::GetUp () const
	{
		return XMLoadFloat3 (&m_Up);
	}

	XMVECTOR View::GetForward () const
	{
		return XMLoadFloat3 (&m_Forward);
	}

	XMVECTOR View::GetRight () const
	{
		return XMLoadFloat3 (&m_Right);
	}

}
