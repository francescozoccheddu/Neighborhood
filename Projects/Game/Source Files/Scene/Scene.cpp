#pragma once

#include <Game/Scene/Scene.hpp>

using namespace DirectX;

void Scene::Transform::Update ()
{
	XMStoreFloat4x4 (&m_Transform, XMMatrixTranspose (XMMatrixAffineTransformation (XMLoadFloat3 (&scale), g_XMZero, XMLoadFloat4 (&rotation), XMLoadFloat3 (&position))));
}

XMMATRIX Scene::Transform::Get () const
{
	return XMLoadFloat4x4 (&m_Transform);
}

const XMFLOAT4X4& Scene::AbstractView::Get () const
{
	return m_mTransform;
}

Scene::ViewWithTarget::ViewWithTarget ()
{
	ViewWithTarget::Update ();
}

void Scene::ViewWithTarget::Update ()
{
	CXMMATRIX transform = XMMatrixTranspose (XMMatrixLookAtLH (XMLoadFloat3 (&position), XMLoadFloat3 (&target), XMLoadFloat3 (&unrotatedUp)));
	XMStoreFloat4x4 (&m_mTransform, transform);
}

void Scene::View::Update ()
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

void Scene::View::Move (float _right, float _up, float _forward, float _speed)
{
	CXMVECTOR newPos = XMLoadFloat3 (&position) + (_right * XMLoadFloat3 (&m_Right) + _up * XMLoadFloat3 (&m_Up) + _forward * XMLoadFloat3 (&m_Forward)) * _speed;
	XMStoreFloat3 (&position, newPos);
}

void Scene::View::ClampLookUp (float _angle)
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

Scene::View::View ()
{
	View::Update ();
}

XMVECTOR Scene::View::GetUp () const
{
	return XMLoadFloat3 (&m_Up);
}

XMVECTOR Scene::View::GetForward () const
{
	return XMLoadFloat3 (&m_Forward);
}

XMVECTOR Scene::View::GetRight () const
{
	return XMLoadFloat3 (&m_Right);
}

float Scene::Projection::CalcAspectRatio (int _w, int _h)
{
	return _w / static_cast<float>(_h);
}

float Scene::Projection::CalcVFov (float _hf, float _a)
{
	float w = std::tan (_hf / 2.0f);
	float h = w / _a;
	return 2.0f * std::atan (h);
}

Scene::Projection::Projection ()
{
	Update ();
}

void Scene::Projection::Update ()
{
	XMStoreFloat4x4 (&m_mTransform, XMMatrixTranspose (XMMatrixPerspectiveFovLH (vFov, aspectRatio, nearZ, farZ)));
}

const XMFLOAT4X4& Scene::Projection::Get () const
{
	return m_mTransform;
}