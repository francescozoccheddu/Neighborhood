#pragma once

#include <Game/DirectXMath.hpp>

namespace Camera
{

	class AbstractView
	{

	public:

		DirectX::XMFLOAT3 unrotatedUp { 0.0f, 1.0f, 0.0f };
		DirectX::XMFLOAT3 position { 0.0f, 0.0f, 0.0f };

		DirectX::XMMATRIX Get () const;

		virtual void Update () = 0;

		virtual ~AbstractView () = default;

	protected:

		DirectX::XMFLOAT4X4 m_mTransform;

	};

	class ViewWithTarget : public AbstractView
	{

	public:

		DirectX::XMFLOAT3 target { 0.0f, 0.0f, 1.0f };

		ViewWithTarget ();

		void Update () override;

	};

	class View : public AbstractView
	{

	public:

		DirectX::XMFLOAT3 unrotatedForward { 0.0f, 0.0f, 1.0f };
		float turn { 0.0f };
		float lookUp { 0.0f };
		float tilt { 0.0f };

		View ();


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

}
