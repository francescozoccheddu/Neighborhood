#pragma once

#include <Game/Scene/Lights.hpp>
#include <Game/Scene/Camera.hpp>
#include <Game/Resources/MeshResource.hpp>
#include <Game/Scene/Transform.hpp>
#include <vector>
#include <string>


struct Scene
{

public:

	struct Drawable
	{
		const SceneMeshResource * pMesh;
		Transform transform;
	};

	std::vector<Drawable> drawables {};
	std::vector<Light*> lights {};
	AbstractView * pView { nullptr };
	AbstractProjection * pProjection { nullptr };

};

