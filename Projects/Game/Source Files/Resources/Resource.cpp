#include <Game/Resources/Resource.hpp>

#include <Game/Utils/Exceptions.hpp>
#include <Game/Utils/Storage.hpp>

FileResource::FileResource (const std::string& _fileName) : m_FileName { _fileName } {}

const std::string& FileResource::GetFileName () const
{
	return m_FileName;
}

BinaryFileResource::~BinaryFileResource ()
{
	if (BinaryFileResource::IsLoaded ())
	{
		BinaryFileResource::Unload ();
	}
}

const void * BinaryFileResource::GetData () const
{
	GAME_ASSERT_MSG (BinaryFileResource::IsLoaded (), "Not loaded");
	return m_pData;
}

int BinaryFileResource::GetSize () const
{
	GAME_ASSERT_MSG (BinaryFileResource::IsLoaded (), "Not loaded");
	return m_cData;
}

void BinaryFileResource::Load ()
{
	GAME_ASSERT_MSG (!BinaryFileResource::IsLoaded (), "Already loaded");
	m_pData = Storage::LoadBinaryFile (GetFileName (), m_cData);
}

void BinaryFileResource::Unload ()
{
	GAME_ASSERT_MSG (BinaryFileResource::IsLoaded (), "Not loaded");
	delete m_pData;
}

bool BinaryFileResource::IsLoaded () const
{
	return m_pData != nullptr;
}

