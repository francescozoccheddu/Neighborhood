#pragma once

#include <DirectXMath.h>

namespace Camera
{

	class AbstractView
	{

	public:

		DirectX::XMVECTOR unrotatedUp{ 0.0f, 1.0f, 0.0f, 0.0f };
		DirectX::XMVECTOR position{ 0.0f, 0.0f, 0.0f, 1.0f };

		DirectX::CXMMATRIX Get () const;

		virtual void Update () = 0;

		virtual ~AbstractView () = default;

	protected:

		DirectX::XMMATRIX m_mTransform;

	};

	class ViewWithTarget : public AbstractView
	{

	public:

		ViewWithTarget ();

		DirectX::XMVECTOR target{ 0.0f, 0.0f, 0.5f, 1.0f };

		void Update () override;

	};

}
