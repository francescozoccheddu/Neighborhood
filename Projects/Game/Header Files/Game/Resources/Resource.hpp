#pragma once

#include <Game/Direct3D.hpp>
#include <Game/Utils/NonCopyable.h>
#include <string>

#define RES_DIR "Resources/"
#define RES_SHADER(_name) RES_DIR "Shaders/" _name ".cso"
#define RES_MESH(_name) RES_DIR "Meshes/" _name ".json"
#define RES_TEXTURE(_name) RES_DIR "Textures/" _name ".dds"


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