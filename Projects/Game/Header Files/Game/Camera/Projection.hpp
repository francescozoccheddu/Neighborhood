#pragma once

#include <Game/DirectXMath.hpp>

namespace Camera
{

	class Projection
	{

	public:

		float vFov { 1.5708f };
		float nearZ { 0.01f };
		float farZ { 100.0f };
		float aspectRatio { 1.0f };

		static float CalcAspectRatio (int width, int height);

		static float CalcVFov (float hFov, float aspectRatio);

		Projection ();

		void Update ();

		DirectX::XMMATRIX Get () const;

		virtual ~Projection () = default;

	private:

		DirectX::XMFLOAT4X4 m_mTransform;

	};

}
