#pragma once

#include <Game/DirectXMath.hpp>

struct Light
{

	enum class Type
	{
		POINT, DIRECTION, CONE
	};

	virtual ~Light () = default;

	DirectX::XMFLOAT3 color;
	DirectX::XMFLOAT3 position;
	bool bCastShadows;
	float nearZ { 0.1f };
	float farZ { 100.0f };

	virtual Type GetType () const = 0;

protected:

	Light () = default;

};

struct PointLight : public Light
{
	DirectX::XMFLOAT3 position { 0.0f, 0.0f, 0.0f };
	float radius { 0.0f };

	inline Type GetType () const override final
	{
		return Type::POINT;
	}
};

struct DirectionalLight : public Light
{
	DirectX::XMFLOAT3 direction { 0.0f, -1.0f, 0.0f };
	float shadowSize;

	inline Type GetType () const override final
	{
		return Type::DIRECTION;
	}
};

struct ConeLight : public Light
{
	DirectX::XMFLOAT3 direction { 0.0f, -1.0f, 0.0f };

	float innerAngle { 0.5f };
	float outerAngle { 1.0f };

	inline Type GetType () const override final
	{
		return Type::CONE;
	}
};