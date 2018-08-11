#pragma once

#include <Game/Direct3D.hpp>
#include <string>

class Resource
{

public:

	Resource () = default;

	Resource (Resource&&);

	Resource& operator=(Resource&&);

	virtual ~Resource () = default;

	Resource (const Resource&) = delete;

	Resource& operator=(const Resource&) = delete;

	bool IsCreated () const;

	void Create (ID3D11Device & device);

	void Destroy ();

protected:

	virtual void DoCreate (ID3D11Device & device) = 0;

	virtual void DoDestroy () = 0;

	void AssertCreated () const;

private:

	bool m_bCreated { false };

};

class LoadableResource : public Resource
{
public:

	LoadableResource () = default;

	LoadableResource (LoadableResource&&);

	LoadableResource& operator=(LoadableResource&&);

	bool IsLoaded () const;

	void Load ();

	void Unload ();

protected:

	virtual void DoCreateAfterLoad (ID3D11Device & device) = 0;

	virtual void DoLoad () = 0;

	virtual void DoUnload () = 0;

	void AssertLoaded () const;

private:

	void DoCreate (ID3D11Device & device) override final;

	bool m_bLoaded { false };

};

class FileResource : public LoadableResource
{

public:

	FileResource (const std::string& fileName);

	const std::string& GetFileName () const;

protected:

	virtual void DoCreateFromFile (ID3D11Device & device) = 0;

	virtual void DoLoad () = 0;

	virtual void DoUnload () = 0;

private:

	void DoCreateAfterLoad (ID3D11Device & device) override final;

	const std::string m_FileName;

};

class BinaryFileResource : public FileResource
{

public:

	using FileResource::FileResource;

	~BinaryFileResource () override;

protected:


	void DoLoad () override final;

	void DoUnload () override final;

	void DoCreateFromFile (ID3D11Device & device) override final;

	virtual void DoCreateFromBinary (ID3D11Device & device, const void * pData, int cData) = 0;

private:

	void * m_pData { nullptr };
	int m_cData;

};