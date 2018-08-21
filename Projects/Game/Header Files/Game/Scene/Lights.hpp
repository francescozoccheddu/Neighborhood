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

	inline Type GetType () const override final
	{
		return Type::DIRECTION;
	}
};

struct ConeLight : public Light
{
	DirectX::XMFLOAT3 position { 0.0f, 1.0f, 0.0f };
	DirectX::XMFLOAT3 target { 0.0f, 0.0f, 0.0f };
	float angle { 0.0f };

	inline Type GetType () const override final
	{
		return Type::CONE;
	}
};