#pragma once

#include <Game/DirectXMath.hpp>

class Transform
{

public:

	DirectX::XMFLOAT3 position { 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 scale { 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT4 rotation { 1.0f, 0.0f, 0.0f, 0.0f };

	void Update ();

	DirectX::XMMATRIX Get () const;

private:

	DirectX::XMFLOAT4X4 m_Transform;

};