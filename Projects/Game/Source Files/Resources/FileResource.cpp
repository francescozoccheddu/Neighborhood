#include <Game/Resources/FileResource.hpp>

#include <Game/Utils/Exceptions.hpp>

void FileResource::DoCreate (ID3D11Device & _device)
{
	GAME_ASSERT_MSG (IsLoaded (), "Not loaded");
	DoCreateFromFile (_device);
}

bool FileResource::IsLoaded () const
{
	return m_bLoaded;
}

void FileResource::Load ()
{
	GAME_ASSERT_MSG (!IsLoaded (), "Already loaded");
	DoLoad ();
	m_bLoaded = true;
}

void FileResource::Unload ()
{
	GAME_ASSERT_MSG (IsLoaded (), "Not loaded");
	DoUnload ();
	m_bLoaded = false;
}

const std::string& FileResource::GetFileName () const
{
	return m_FileName;
}

FileResource::FileResource (const std::string& _fileName) : m_FileName { _fileName } {}