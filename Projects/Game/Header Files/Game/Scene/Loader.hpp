#pragma once

#include <Game/Scene/Mesh.hpp>
#include <map>
#include <string>

namespace Scene
{

	std::map<std::string, Mesh*> LoadFromJSON (const std::string& json);

}