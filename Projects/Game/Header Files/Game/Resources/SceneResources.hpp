#pragma once

#include <Game/Resources/Resource.hpp>
#include <Game/Resources/MeshResource.hpp>
#include <Game/Resources/TextureResource.hpp>
#include <string>
#include <unordered_map>
#include <Game/Utils/Exceptions.hpp>

#define _SCENERESOURCES_DECL(_type,_typename) \
public: inline const _type & Get##_typename(const std::string& _name) { AssertCreated(); return m_Map##_type.Get(_name,m_Moment,*m_pDevice); }; \
private: MeteredResourceMap<_type> m_Map##_type;

class SceneResources : public Resource
{

public:

	void Tick ();

	void DestroyUnusedResources (int maxRestMoments);

protected:

	void DoCreate (ID3D11Device & device) override;

	void DoDestroy () override;

private:

	using moment_t = int;

	template<typename T>
	class MeteredResourceMap
	{

	public:

		T & Get (const std::string& _name, moment_t _moment, ID3D11Device &_pDevice)
		{
			auto iterator { m_Map.find (_name) };
			if (iterator == m_Map.end ())
			{
				iterator = m_Map.emplace (_name, Entry { new T { _name }, 0 }).first;
			}
			Entry & entry { iterator->second };
			entry.lastMoment = _moment;
			if (!entry.resource->IsLoaded ())
			{
				entry.resource->Load ();
			}
			if (!entry.resource->IsCreated ())
			{
				entry.resource->Create (_pDevice);
			}
			return *entry.resource;
		}

		void DestroyOlder (moment_t _moment)
		{
			for (auto &[key, entry] : m_Map)
			{
				if (entry.lastMoment < _moment && entry.resource->IsCreated ())
				{
					entry.resource->Destroy ();
				}
			}
		}

		void CreateAll (ID3D11Device & _device)
		{
			for (auto &[key, entry] : m_Map)
			{
				entry.resource->Create (_device);
			}
		}

		void DestroyAll ()
		{
			for (auto &[key, entry] : m_Map)
			{
				if (entry.resource->IsCreated ())
				{
					entry.resource->Destroy ();
				}
			}
		}

		~MeteredResourceMap ()
		{
			for (auto &[key, entry] : m_Map)
			{
				delete entry.resource;
			}
		}

	private:

		struct Entry
		{
			T* resource;
			moment_t lastMoment;
		};

		std::unordered_map<std::string, Entry> m_Map;

	};

	moment_t m_Moment { 0 };
	ID3D11Device * m_pDevice;

	_SCENERESOURCES_DECL (MeshResource, Mesh);
	_SCENERESOURCES_DECL (TextureResource, Texture);

};