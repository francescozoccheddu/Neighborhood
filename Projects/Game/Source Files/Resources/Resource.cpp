#include <Game/Resources/Resource.hpp>

#include <Game/Utils/Exceptions.hpp>
#include <Game/Utils/Storage.hpp>

Resource::Resource (Resource&& _resource)
{
	m_bCreated = _resource.m_bCreated;
	_resource.m_bCreated = false;
}

Resource& Resource::operator=(Resource&& _resource)
{
	m_bCreated = _resource.m_bCreated;
	_resource.m_bCreated = false;
	return *this;
}

bool Resource::IsCreated () const
{
	return m_bCreated;
}

void Resource::Create (ID3D11Device & _device)
{
	GAME_ASSERT_MSG (!IsCreated (), "Already created");
	DoCreate (_device);
	m_bCreated = true;
}

void Resource::Destroy ()
{
	AssertCreated ();
	DoDestroy ();
	m_bCreated = false;
}

void Resource::AssertCreated () const
{
	GAME_ASSERT_MSG (IsCreated (), "Not created");
}

LoadableResource::LoadableResource (LoadableResource && _resource) : Resource (static_cast<Resource&&>(_resource))
{
	m_bLoaded = _resource.m_bLoaded;
	_resource.m_bLoaded = false;
}

LoadableResource& LoadableResource::operator= (LoadableResource && _resource)
{
	Resource::operator=(static_cast<Resource&&>(_resource));
	m_bLoaded = _resource.m_bLoaded;
	_resource.m_bLoaded = false;
	return *this;
}

void LoadableResource::DoCreate (ID3D11Device & _device)
{
	AssertLoaded ();
	DoCreateAfterLoad (_device);
}

bool LoadableResource::IsLoaded () const
{
	return m_bLoaded;
}

void LoadableResource::Load ()
{
	GAME_ASSERT_MSG (!IsLoaded (), "Already loaded");
	DoLoad ();
	m_bLoaded = true;
}

void LoadableResource::Unload ()
{
	AssertLoaded ();
	DoUnload ();
	m_bLoaded = false;
}

void LoadableResource::AssertLoaded () const
{
	GAME_ASSERT_MSG (IsLoaded (), "Not loaded");
}

void FileResource::DoCreateAfterLoad (ID3D11Device & _device)
{
	GAME_ASSERT_MSG (IsLoaded (), "Not loaded");
	DoCreateFromFile (_device);
}

const std::string& FileResource::GetFileName () const
{
	return m_FileName;
}

FileResource::FileResource (const std::string& _fileName) : m_FileName { _fileName } {}

BinaryFileResource::~BinaryFileResource ()
{
	BinaryFileResource::DoUnload ();
}

void BinaryFileResource::DoLoad ()
{
	m_pData = Storage::LoadBinaryFile (GetFileName (), m_cData);
}

void BinaryFileResource::DoUnload ()
{
	if (m_pData)
	{
		delete m_pData;
		m_pData = nullptr;
	}
}

void BinaryFileResource::DoCreateFromFile (ID3D11Device & _device)
{
	DoCreateFromBinary (_device, m_pData, m_cData);
}