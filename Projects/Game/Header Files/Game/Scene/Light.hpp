#pragma once

#include <Game/DirectXMath.hpp>

struct Light
{

	virtual ~Light () = default;

	DirectX::XMFLOAT3 color;
	bool bCastShadows;

protected:

	Light () = default;

};

struct PointLight : public Light
{
	DirectX::XMFLOAT3 position;
	float radius;
};

struct DirectionalLight : public Light
{
	DirectX::XMFLOAT3 direction;
};

struct ConeLight : public Light
{
	DirectX::XMFLOAT3 position;
	DirectX::XMFLOAT3 target;
	float angle;
};