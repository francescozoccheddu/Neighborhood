#pragma once

#include <Game/Resources/Resource.hpp>
#include <Game/Resources/MeshResource.hpp>
#include <Game/Resources/ShaderResource.hpp>
#include <Game/Resources/TextureResource.hpp>
#include <string>
#include <map>

class ResourceHolder : public Resource
{

public:

	void AddShader (const std::string& name, ShaderResource::ShaderType type);

	void AddMesh (const std::string& name);

	void AddTexture (const std::string& name);

	void RemoveShader (const std::string& name);

	void RemoveMesh (const std::string& name);

	void RemoveTexture (const std::string& name);

	const MeshResource & GetMesh (const std::string& name) const;

	const TextureResource & GetTexture (const std::string& name) const;

	const ShaderResource & GetShader (const std::string& name) const;

	bool HasShader (const std::string& name) const;

	bool HasMesh (const std::string& name) const;

	bool HasTexture (const std::string& name) const;

protected:

	void DoLoad () override;

	void DoUnload () override;

	void DoCreate (ID3D11Device & device) override;

	void DoDestroy () override;

private:

	ID3D11Device * m_pDevice;
	std::map<std::string, MeshResource> m_Meshes;
	std::map<std::string, TextureResource> m_Textures;
	std::map<std::string, ShaderResource> m_Shaders;

	void OnInsert (Resource & resource);

	void OnRemove (Resource & resource);

};