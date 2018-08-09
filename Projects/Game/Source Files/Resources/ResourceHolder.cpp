#include <Game/Resources/ResourceHolder.hpp>
#include <Game/Utils/Exceptions.hpp>

#define _RESOURCES_DIR "Resources/"
#define _MESHES_DIR _RESOURCES_DIR "Meshes/"
#define _TEXTURES_DIR _RESOURCES_DIR "Textures/"
#define _SHADERS_DIR _RESOURCES_DIR "Shaders/"
#define _SCENES_DIR _RESOURCES_DIR "Scenes/"

#define _INSERT(_map,_key,_val) { auto val{_val}; GAME_ASSERT_MSG(_map.emplace(_key,val).second,"Already added"); OnInsert(val); }

void ResourceHolder::AddMesh (const std::string& _name) _INSERT (m_Meshes, _name, MeshResource (_MESHES_DIR + _name));

void ResourceHolder::AddTexture (const std::string& _name) _INSERT (m_Textures, _name, TextureResource (_TEXTURES_DIR + _name));

void ResourceHolder::AddShader (const std::string& _name, ShaderResource::ShaderType _eType) _INSERT (m_Shaders, _name, ShaderResource (_SHADERS_DIR + _name, _eType));

#define _REMOVE(_map,_key) { auto it {_map.find(_key)}; GAME_ASSERT_MSG(it != _map.end(),"Already removed"); OnRemove(it->second); _map.erase(it); }

void ResourceHolder::RemoveMesh (const std::string& _name) _REMOVE (m_Meshes, _name);

void ResourceHolder::RemoveShader (const std::string& _name) _REMOVE (m_Shaders, _name);

void ResourceHolder::RemoveTexture (const std::string& _name) _REMOVE (m_Textures, _name);

#define _HAS(_map,_key) { return _map.find(_key) != _map.end(); }

bool ResourceHolder::HasMesh (const std::string& _name) const _HAS (m_Meshes, _name);

bool ResourceHolder::HasShader (const std::string& _name) const _HAS (m_Shaders, _name);

bool ResourceHolder::HasTexture (const std::string& _name) const _HAS (m_Textures, _name);

#define _GET(_map,_key) { auto it {_map.find(_key)}; GAME_ASSERT_MSG(it != _map.end(),"Not contained"); return it->second; }

const MeshResource& ResourceHolder::GetMesh (const std::string& _name) const _GET (m_Meshes, _name);

const ShaderResource& ResourceHolder::GetShader (const std::string& _name) const _GET (m_Shaders, _name);

const TextureResource& ResourceHolder::GetTexture (const std::string& _name) const _GET (m_Textures, _name);

void ResourceHolder::OnInsert (Resource & _resource)
{
	if (IsLoaded ())
	{
		_resource.Load ();
	}
	if (IsCreated ())
	{
		_resource.Create (*m_pDevice);
	}
}

void ResourceHolder::OnRemove (Resource & _resource)
{
	if (_resource.IsLoaded ())
	{
		_resource.Unload ();
	}
	if (_resource.IsCreated ())
	{
		_resource.Destroy ();
	}
}

#define _FOREACH(_map,_what) { for (auto it = _map.begin(); it != _map.end(); ++it) { it->second. _what ;} }
#define _DO(_what) { _FOREACH(m_Meshes,_what); _FOREACH(m_Textures,_what); _FOREACH(m_Shaders,_what); }

void ResourceHolder::DoLoad ()
{
	_DO (Load ());
}

void ResourceHolder::DoUnload ()
{
	_DO (Unload ());
}

void ResourceHolder::DoCreate (ID3D11Device & _device)
{
	_DO (Create (_device));
}

void ResourceHolder::DoDestroy ()
{
	_DO (Destroy ());
}