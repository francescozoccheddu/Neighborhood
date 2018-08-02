#include <Game/Camera/Projection.hpp>

#include <DirectXMath.h>
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
		float w = tan (_hf / 2.0f);
		float h = w / _a;
		return 2.0f * atan (h);
	}

	Projection::Projection ()
	{
		Update ();
	}

	void Projection::Update ()
	{
		m_mTransform = XMMatrixTranspose (XMMatrixPerspectiveFovLH (vFov, aspectRatio, nearZ, farZ));
	}

	CXMMATRIX Projection::Get () const
	{
		return m_mTransform;
	}

}
