#include <Game/Scene/Lights.hpp>

#include <Game/Utils/Exceptions.hpp>

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
	proj.vFov = outerAngle * 2.0f;
	proj.Update ();
	ViewWithDirection view;
	view.position = position;
	view.direction = direction;
	view.Update ();
	m_Matrix = proj * view;
}
