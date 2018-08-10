#pragma once

#include <Game/Resources/Resource.hpp>
#include <Game/Resources/MeshResource.hpp>
#include <Game/Resources/ShaderResource.hpp>
#include <Game/Resources/TextureResource.hpp>
#include <unordered_map>
#include <string>

#define _GAME_RESOURCEHOLDER_MAP_NAME(_type) m_Map##_type
#define _GAME_RESOURCEHOLDER_MAP_DEF(_type) std::unordered_map<std::string,_type> _GAME_RESOURCEHOLDER_MAP_NAME;

class ResourceHolder : public Resource
{

public:

private:

	_GAME_RESOURCEHOLDER_MAP_DEF (MeshResource);
	_GAME_RESOURCEHOLDER_MAP_DEF (ShaderResource);
	_GAME_RESOURCEHOLDER_MAP_DEF (TextureResource);

};