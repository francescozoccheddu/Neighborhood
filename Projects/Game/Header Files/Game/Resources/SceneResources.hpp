#pragma once

#include <Game/Resources/Resource.hpp>
#include <Game/Resources/MeshResource.hpp>
#include <Game/Resources/TextureResource.hpp>
#include <Game/Utils/Exceptions.hpp>
#include <unordered_map>

class SceneResources : public Resource
{

public:

	using moment_t = int;

	~SceneResources ();

	void Tick ();

	void DestroyUnusedResources (moment_t maxRestMoments);

	void UnloadUnusedResources (moment_t maxRestMoments);

	void Create (ID3D11Device & device) override final;

	void Destroy () override final;

	bool IsCreated () const override final;

	const MeshResource& GetMesh (const std::string& name);

	const TextureResource& GetTexture (const std::string& name);

private:

	struct Entry
	{
		LoadableResource * const pResource;
		moment_t lastUsageMoment;
	};

	using map_t = std::unordered_map<std::string, Entry>;

	moment_t m_Moment { 0 };
	ID3D11Device * m_pDevice { nullptr };

	map_t m_Meshes;
	map_t m_Textures;

	static void DestroyUnusedResources (map_t& map, moment_t minMoment);

	static void UnloadUnusedResources (map_t& map, moment_t minMoment);

	void Destroy (map_t& map);

	template<typename T>
	const T& Get (map_t& _map, const std::string& _name)
	{
		GAME_ASSERT_MSG (IsCreated (), "Not created");
		auto it { _map.find (_name) };
		if (it == _map.end ())
		{
			T * pRes { new T (_name) };
			it = _map.emplace (_name, Entry { pRes, 0 }).first;
		}
		Entry & entry { it->second };
		entry.lastUsageMoment = m_Moment;
		T& res { *reinterpret_cast<T*>(entry.pResource) };
		if (!res.IsLoaded ())
		{
			res.Load ();
		}
		if (!res.IsCreated ())
		{
			res.Create (*m_pDevice);
		}
		return res;
	}

};