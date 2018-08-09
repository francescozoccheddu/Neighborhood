#pragma once

#include <Game/DirectXMath.hpp>
#include <Game/Scene/Transform.hpp>
#include <Game/Scene/Material.hpp>
#include <Game/Scene/Light.hpp>
#include <Game/Resources/MeshResource.hpp>
#include <vector>

struct Scene
{

	struct Drawable
	{
		const MeshResource & mesh;
		Transform transform;
		Material material;
	};

	std::vector<Drawable> drawables;
	std::vector<PointLight> pointLights;
	std::vector<DirectionalLight> directionalLights;
	std::vector<ConeLight> coneLight;

};