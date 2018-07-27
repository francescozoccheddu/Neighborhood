#include <Game/Camera/View.hpp>

#include <DirectXMath.h>

namespace Camera
{

	DirectX::CXMMATRIX AbstractView::Get () const
	{
		return m_mTransform;
	}

	ViewWithTarget::ViewWithTarget ()
	{
		ViewWithTarget::Update ();
	}

	void ViewWithTarget::Update ()
	{
		m_mTransform = DirectX::XMMatrixTranspose (DirectX::XMMatrixLookAtLH (position, target, unrotatedUp));
	}

}
