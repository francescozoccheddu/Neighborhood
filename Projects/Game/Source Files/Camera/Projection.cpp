#include <Game/Camera/Projection.hpp>

#include <cmath>

using namespace DirectX;

namespace Camera
{

	float Projection::CalcAspectRatio (int _w, int _h)
	{
		return _w / static_cast<float>(_h);
	}

	float Projection::CalcVFov (float _hf, float _a)
	{
		float w = std::tan (_hf / 2.0f);
		float h = w / _a;
		return 2.0f * std::atan (h);
	}

	Projection::Projection ()
	{
		Update ();
	}

	void Projection::Update ()
	{
		XMStoreFloat4x4 (&m_mTransform, XMMatrixTranspose (XMMatrixPerspectiveFovLH (vFov, aspectRatio, nearZ, farZ)));
	}

	XMMATRIX Projection::Get () const
	{
		return XMLoadFloat4x4 (&m_mTransform);
	}

}
