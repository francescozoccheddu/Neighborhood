#include <Game/Resources/SceneResources.hpp>

SceneResources::~SceneResources ()
{
	if (SceneResources::IsCreated ())
	{
		SceneResources::Destroy ();
	}
}

void SceneResources::Tick ()
{
	m_Moment++;
}

void SceneResources::DestroyUnusedResources (moment_t _maxRestMoments)
{
	moment_t minMoment { m_Moment - _maxRestMoments };
	DestroyUnusedResources (m_Meshes, minMoment);
	DestroyUnusedResources (m_Textures, minMoment);
}

void SceneResources::UnloadUnusedResources (moment_t _maxRestMoments)
{
	moment_t minMoment { m_Moment - _maxRestMoments };
	UnloadUnusedResources (m_Meshes, minMoment);
	UnloadUnusedResources (m_Textures, minMoment);
}

void SceneResources::Create (ID3D11Device & _device)
{
	GAME_ASSERT_MSG (!SceneResources::IsCreated (), "Already created");
	m_pDevice = &_device;
}

void SceneResources::Destroy ()
{
	GAME_ASSERT_MSG (SceneResources::IsCreated (), "Not created");
	m_pDevice = nullptr;
	Destroy (m_Meshes);
	Destroy (m_Textures);
}

bool SceneResources::IsCreated () const
{
	return m_pDevice != nullptr;
}

const SceneMeshResource & SceneResources::GetMesh (const std::string & _name) const
{
	return Get<SceneMeshResource> (m_Meshes, _name, GetMeshFileName (_name));
}

const TextureResource & SceneResources::GetTexture (const std::string & _name) const
{
	return Get<TextureResource> (m_Textures, _name, GetTextureFileName (_name));
}

void SceneResources::DestroyUnusedResources (map_t& _map, moment_t _minMoment)
{
	for (auto&[key, entry] : _map)
	{
		if (entry.lastUsageMoment < _minMoment && entry.pResource->IsCreated ())
		{
			entry.pResource->Destroy ();
		}
	}
}

void SceneResources::UnloadUnusedResources (map_t& _map, moment_t _minMoment)
{
	for (auto&[key, entry] : _map)
	{
		if (entry.lastUsageMoment < _minMoment && entry.pResource->IsLoaded ())
		{
			entry.pResource->Unload ();
		}
	}
}

void SceneResources::Destroy (map_t& _map)
{
	for (auto&[key, entry] : _map)
	{
		if (entry.pResource->IsCreated ())
		{
			entry.pResource->Destroy ();
		}
	}
}