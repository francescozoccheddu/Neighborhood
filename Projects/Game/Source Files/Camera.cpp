#include <Game/Scene/Camera.hpp>

#include <cmath>

using namespace DirectX;

const XMFLOAT4X4& AbstractView::Get () const
{
	return m_mTransform;
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
	CXMVECTOR newPos = XMLoadFloat3 (&position) + (_right * XMLoadFloat3 (&m_Right) + _up * XMLoadFloat3 (&m_Up) + _forward * XMLoadFloat3 (&m_Forward)) * _speed;
	XMStoreFloat3 (&position, newPos);
}

void View::ClampLookUp (float _angle)
{
	if (lookUp > _angle)
	{
		lookUp = _angle;
	}
	else if (lookUp < -_angle)
	{
		lookUp = -_angle;
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

float Projection::CalcAspectRatio (int _w, int _h)
{
	return _w / static_cast<float>(_h);
}

float Projection::CalcVFov (float _hf, float _a)
{
	float w = std::tan (_hf / 2.0f);
	float h = w / _a;
	return 2.0f * std::atan (h);
}

Projection::Projection ()
{
	Update ();
}

void Projection::Update ()
{
	XMStoreFloat4x4 (&m_mTransform, XMMatrixTranspose (XMMatrixPerspectiveFovLH (vFov, aspectRatio, nearZ, farZ)));
}

const XMFLOAT4X4& Projection::Get () const
{
	return m_mTransform;
}