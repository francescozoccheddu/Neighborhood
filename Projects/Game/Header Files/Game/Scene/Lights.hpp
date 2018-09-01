#pragma once

#include <Game/Utils/MatrixWrapper.hpp>
#include <Game/DirectXMath.hpp>
#include <Game/Direct3D.hpp>
#include <Game/Scene/Camera.hpp>

struct Light : public MatrixWrapper
{

	enum class Type
	{
		POINT, DIRECTION, CONE
	};

	DirectX::XMFLOAT3 color { 1.0f, 1.0f, 1.0f };
	DirectX::XMFLOAT3 position { 0.0f, 0.0f, 0.0f };
	bool bCastShadows { false };
	float shadowNearZ { 0.5f };
	float shadowFarZ { 10.0f };
	float intensity { 1.0f };

	virtual Type GetType () const = 0;

	virtual void Update () = 0;

protected:

	Light () = default;

};

struct PointLight : public Light
{
	DirectX::XMFLOAT3 radius { 1.0f, 1.0f, 1.0f };

	void Update () override final;

	ViewWithDirection CalcView (D3D11_TEXTURECUBE_FACE face) const;

	inline Type GetType () const override final
	{
		return Type::POINT;
	}
};

struct DirectionalLight : public Light
{
	DirectX::XMFLOAT3 direction { 0.0f, -1.0f, 0.0f };
	float shadowSize { 2.0f };

	void Update () override final;

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
	float startLength { 0.1f };
	float endLength { 2.0f };
	float realEndLength { 2.5f };

	void Update () override final;

	inline Type GetType () const override final
	{
		return Type::CONE;
	}
};