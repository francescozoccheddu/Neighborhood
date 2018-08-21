#pragma once

#include <Game/Scene/Lights.hpp>
#include <Game/Scene/Camera.hpp>
#include <Game/Scene/Transform.hpp>
#include <vector>
#include <string>


struct Scene
{

public:

	struct Drawable
	{
		std::string mesh;
		Transform transform;
		std::string texture;
	};

	std::vector<Drawable> drawables {};
	std::vector<PointLight> pointLights {};
	std::vector<DirectionalLight> directionalLights {};
	std::vector<ConeLight> coneLight {};
	AbstractView * pView { nullptr };
	AbstractProjection * pProjection { nullptr };

};

