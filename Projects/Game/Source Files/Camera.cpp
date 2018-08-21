#include <Game/Scene/Camera.hpp>

#include <cmath>
#include "..\Header Files\Game\Scene\Camera.hpp"

using namespace DirectX;

void ViewWithTarget::Update ()
{
	CXMMATRIX transform = XMMatrixTranspose (XMMatrixLookAtLH (XMLoadFloat3 (&position), XMLoadFloat3 (&target), XMLoadFloat3 (&unrotatedUp)));
	XMStoreFloat4x4 (&m_Matrix, transform);
}

void ViewWithDirection::Update ()
{
	CXMMATRIX transform = XMMatrixTranspose (XMMatrixLookToLH (XMLoadFloat3 (&position), XMLoadFloat3 (&direction), XMLoadFloat3 (&unrotatedUp)));
	XMStoreFloat4x4 (&m_Matrix, transform);
}

void ViewWithOrientation::Update ()
{
	CXMVECTOR quat { XMQuaternionRotationRollPitchYaw (lookUp, turn, tilt) };
	CXMVECTOR up { XMVector3Rotate (XMLoadFloat3 (&unrotatedUp), quat) };
	CXMVECTOR forward { XMVector3Rotate (XMLoadFloat3 (&unrotatedForward), quat) };
	CXMVECTOR right { XMVector3Cross (up, forward) };
	CXMMATRIX transform = XMMatrixTranspose (XMMatrixLookToLH (XMLoadFloat3 (&position), forward, up));
	XMStoreFloat3 (&m_Up, up);
	XMStoreFloat3 (&m_Forward, forward);
	XMStoreFloat3 (&m_Right, right);
	XMStoreFloat4x4 (&m_Matrix, transform);
}

void ViewWithOrientation::Move (float _right, float _up, float _forward, float _speed)
{
	CXMVECTOR newPos = XMLoadFloat3 (&position) + (_right * XMLoadFloat3 (&m_Right) + _up * XMLoadFloat3 (&m_Up) + _forward * XMLoadFloat3 (&m_Forward)) * _speed;
	XMStoreFloat3 (&position, newPos);
}

void ViewWithOrientation::ClampLookUp (float _angle)
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

XMVECTOR ViewWithOrientation::GetUp () const
{
	return XMLoadFloat3 (&m_Up);
}

XMVECTOR ViewWithOrientation::GetForward () const
{
	return XMLoadFloat3 (&m_Forward);
}

XMVECTOR ViewWithOrientation::GetRight () const
{
	return XMLoadFloat3 (&m_Right);
}

float AbstractProjection::CalcAspectRatio (float _w, float _h)
{
	return _w / _h;
}

float OrthographicProjection::CalcHeight (float _w, float _a)
{
	return _w / _a;
}

void OrthographicProjection::Update ()
{
	XMStoreFloat4x4 (&m_Matrix, XMMatrixTranspose (XMMatrixOrthographicLH (height * aspectRatio, height, nearZ, farZ)));
}

float PerspectiveProjection::CalcVFov (float _hf, float _a)
{
	float w = std::tan (_hf / 2.0f);
	float h = w / _a;
	return 2.0f * std::atan (h);
}

void PerspectiveProjection::Update ()
{
	XMStoreFloat4x4 (&m_Matrix, XMMatrixTranspose (XMMatrixPerspectiveFovLH (vFov, aspectRatio, nearZ, farZ)));
}
