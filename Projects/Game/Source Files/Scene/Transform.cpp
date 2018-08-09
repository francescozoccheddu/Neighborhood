#include <Game/Scene/Transform.hpp>

using namespace DirectX;

void Transform::Update ()
{
	XMStoreFloat4x4 (&m_Transform, XMMatrixTranspose (XMMatrixAffineTransformation (XMLoadFloat3 (&scale), g_XMZero, XMLoadFloat4 (&rotation), XMLoadFloat3 (&position))));
}

XMMATRIX Transform::Get () const
{
	return XMLoadFloat4x4 (&m_Transform);
}