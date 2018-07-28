#pragma once

#include <DirectXMath.h>
#include "../Input/Gamepad.hpp"

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

		DirectX::XMVECTOR target{ 0.0f, 0.0f, 1.0f, 1.0f };

		ViewWithTarget ();

		void Update () override;

	};

	class View : public AbstractView
	{

	public:

		float turn{ 0.0f };
		float lookUp{ 0.0f };
		float tilt{ 0.0f };

		View ();

		DirectX::XMVECTOR unrotatedForward{ 0.0f, 0.0f, 1.0f, 0.0f };

		void Update () override;

		DirectX::CXMVECTOR GetUp () const;

		DirectX::CXMVECTOR GetForward () const;

		DirectX::CXMVECTOR GetRight () const;

		void UpdateFlyingView (float deltaTime, const Gamepad& gamepad);

	private:

		DirectX::XMVECTOR m_Up{ 0.0f, 1.0f, 0.0f, 0.0f };
		DirectX::XMVECTOR m_Forward{ 0.0f, 0.0f, 1.0f, 0.0f };
		DirectX::XMVECTOR m_Right{ 1.0f, 0.0f, 0.0f, 0.0f };

	};

}
