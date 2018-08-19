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
	DirectX::XMFLOAT3 position { 0.0f, 0.0f, 0.0f };
	float radius { 0.0f };
};

struct DirectionalLight : public Light
{
	DirectX::XMFLOAT3 direction { 0.0f, -1.0f, 0.0f };
};

struct ConeLight : public Light
{
	DirectX::XMFLOAT3 position { 0.0f, 1.0f, 0.0f };
	DirectX::XMFLOAT3 target { 0.0f, 0.0f, 0.0f };
	float angle { 0.0f };
};