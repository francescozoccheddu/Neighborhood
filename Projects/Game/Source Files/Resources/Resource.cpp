#include <Game/Resources/Resource.hpp>

#include <Game/Utils/Exceptions.hpp>

bool Resource::IsLoaded () const
{
	return m_bLoaded;
}

void Resource::Load ()
{
	GAME_ASSERT_MSG (!IsLoaded (), "Already loaded");
	DoLoad ();
	m_bLoaded = true;
}

void Resource::Unload ()
{
	GAME_ASSERT_MSG (IsLoaded (), "Not loaded");
	DoUnload ();
	m_bLoaded = false;
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

