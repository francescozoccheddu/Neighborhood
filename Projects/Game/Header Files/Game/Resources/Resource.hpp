#pragma once

#include <Game/Direct3D.hpp>
#include <Game/Utils/NonCopyable.h>
#include <string>

#define RES_DIR "Resources/"
#define RES_SHADERS_DIR RES_DIR "Shaders/"
#define RES_TEXTURES_DIR RES_DIR "Textures/"
#define RES_MESHES_DIR RES_DIR "Meshes/"
#define RES_SHADERS_EXT ".cso"
#define RES_TEXTURES_EXT ".dds"
#define RES_MESHES_EXT ".json"

inline std::string GetShaderFileName (const std::string& _name)
{
	return RES_SHADERS_DIR + _name + RES_SHADERS_EXT;
}

inline std::string GetMeshFileName (const std::string& _name)
{
	return RES_MESHES_DIR + _name + RES_MESHES_EXT;
}

inline std::string GetTextureFileName (const std::string& _name)
{
	return RES_TEXTURES_DIR + _name + RES_TEXTURES_EXT;
}

class Resource : private NonCopyable
{

public:

	virtual void Create (ID3D11Device & device) = 0;

	virtual void Destroy () = 0;

	virtual bool IsCreated () const = 0;

};

class LoadableResource : public Resource
{

public:

	virtual void Load () = 0;

	virtual void Unload () = 0;

	virtual bool IsLoaded () const = 0;

};

class FileResource : public LoadableResource
{

public:

	FileResource (const std::string & _fileName);

	const std::string& GetFileName () const;

private:

	const std::string m_FileName;

};

class BinaryFileResource : public FileResource
{

public:

	using FileResource::FileResource;

	~BinaryFileResource ();

	void Load () override final;

	void Unload () override final;

	bool IsLoaded () const override final;

protected:

	const void * GetData () const;

	int GetSize () const;

private:

	void * m_pData { nullptr };
	int m_cData { 0 };

};