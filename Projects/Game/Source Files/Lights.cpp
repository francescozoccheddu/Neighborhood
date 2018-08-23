#include <Game/Scene/Lights.hpp>

#include <Game/Utils/Exceptions.hpp>

void PointLight::Update ()
{
	DirectX::XMStoreFloat4x4 (&m_Matrix, DirectX::XMMatrixMultiply (CalcProjection (), CalcView (D3D11_TEXTURECUBE_FACE_POSITIVE_Z)));
}

PerspectiveProjection PointLight::CalcProjection () const
{
	PerspectiveProjection proj;
	proj.aspectRatio = 1.0f;
	proj.farZ = shadowFarZ;
	proj.nearZ = shadowNearZ;
	proj.vFov = DirectX::XMConvertToRadians (90.0f);
	proj.Update ();
	return proj;
}

ViewWithTarget PointLight::CalcView (D3D11_TEXTURECUBE_FACE _face) const
{
	ViewWithTarget view;
	view.position = position;
	switch (_face)
	{
		case D3D11_TEXTURECUBE_FACE_POSITIVE_X:
			view.target = { 1.0f, 0.0f, 0.0f };
			view.unrotatedUp = { 0.0f, 1.0f, 0.0f };
			break;
		case D3D11_TEXTURECUBE_FACE_NEGATIVE_X:
			view.target = { -1.0f, 0.0f, 0.0f };
			view.unrotatedUp = { 0.0f, 1.0f, 0.0f };
			break;
		case D3D11_TEXTURECUBE_FACE_POSITIVE_Y:
			view.target = { 0.0f, 1.0f, 0.0f };
			view.unrotatedUp = { 0.0f, 0.0f, -1.0f };
			break;
		case D3D11_TEXTURECUBE_FACE_NEGATIVE_Y:
			view.target = { 0.0f, -1.0f, 0.0f };
			view.unrotatedUp = { 0.0f, 0.0f, 1.0f };
			break;
		case D3D11_TEXTURECUBE_FACE_POSITIVE_Z:
			view.target = { 0.0f, 0.0f, 1.0f };
			view.unrotatedUp = { 0.0f, 1.0f, 0.0f };
			break;
		case D3D11_TEXTURECUBE_FACE_NEGATIVE_Z:
			view.target = { 0.0f, 0.0f, -1.0f };
			view.unrotatedUp = { 0.0f, 1.0f, 0.0f };
			break;
		default:
			GAME_THROW_MSG ("Unknown face");
			break;
	}
	view.Update ();
	return view;
}

void DirectionalLight::Update ()
{
	OrthographicProjection proj;
	proj.aspectRatio = 1.0f;
	proj.farZ = shadowFarZ;
	proj.nearZ = shadowNearZ;
	proj.height = shadowSize;
	proj.Update ();
	ViewWithDirection view;
	view.position = position;
	view.direction = direction;
	view.Update ();
	m_Matrix = proj * view;
}

void ConeLight::Update ()
{
	PerspectiveProjection proj;
	proj.aspectRatio = 1.0f;
	proj.farZ = shadowFarZ;
	proj.nearZ = shadowNearZ;
	proj.vFov = outerAngle;
	proj.Update ();
	ViewWithTarget view;
	view.position = position;
	view.target = target;
	view.Update ();
	m_Matrix = proj * view;
}
