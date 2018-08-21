#pragma once

#include <Game/Utils/MatrixWrapper.hpp>
#include <Game/DirectXMath.hpp>

class AbstractView : public MatrixWrapper
{

public:

	DirectX::XMFLOAT3 unrotatedUp { 0.0f, 1.0f, 0.0f };
	DirectX::XMFLOAT3 position { 0.0f, 0.0f, 0.0f };

	virtual void Update () = 0;

};

class ViewWithTarget : public AbstractView
{

public:

	DirectX::XMFLOAT3 target { 0.0f, 0.0f, 1.0f };

	void Update () override;

};

class ViewWithOrientation : public AbstractView
{

public:

	DirectX::XMFLOAT3 unrotatedForward { 0.0f, 0.0f, 1.0f };
	float turn { 0.0f };
	float lookUp { 0.0f };
	float tilt { 0.0f };

	void Update () override;

	DirectX::XMVECTOR GetUp () const;

	DirectX::XMVECTOR GetForward () const;

	DirectX::XMVECTOR GetRight () const;

	void Move (float right, float up, float forward, float speed = 1.0f);

	void ClampLookUp (float angle);

private:

	DirectX::XMFLOAT3 m_Up { 0.0f, 1.0f, 0.0f };
	DirectX::XMFLOAT3 m_Forward { 0.0f, 0.0f, 1.0f };
	DirectX::XMFLOAT3 m_Right { 1.0f, 0.0f, 0.0f };

};

class AbstractProjection : public MatrixWrapper
{
public:

	float nearZ { 0.01f };
	float farZ { 100.0f };
	float aspectRatio { 1.0f };

	static float CalcAspectRatio (float width, float height);

	virtual void Update () = 0;

};

class OrthographicProjection : public AbstractProjection
{

public:

	static float CalcHeight (float width, float aspectRatio);

	float height { 1.0f };

	void Update () override;

};

class PerspectiveProjection : public OrthographicProjection
{

public:

	float vFov { 1.5708f };

	static float CalcVFov (float hFov, float aspectRatio);

	void Update () override;

};