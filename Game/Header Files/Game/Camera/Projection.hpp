#pragma once

#include <DirectXMath.h>

namespace Camera
{

class Projection
{

public:

	float vFov{ 1.5708f };
	float nearZ{ 1.0f };
	float farZ{ 100.0f };
	float aspectRatio{ 1.0f };

	static float CalcAspectRatio (int width, int height);

	static float CalcVFov (float hFov, float aspectRatio);

	Projection ();

	void Update ();

	DirectX::CXMMATRIX Get () const;

	virtual ~Projection () = default;

private:

	DirectX::XMMATRIX m_mTransform;

};

}
