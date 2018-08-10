#include <Game/Resources/BinaryFileResource.hpp>

#include <Game/Utils/Storage.hpp>
#include <Game/Utils/Exceptions.hpp>

void BinaryFileResource::DoLoad ()
{
	m_pData = Storage::LoadBinaryFile (GetFileName (), m_cData);
}

void BinaryFileResource::DoUnload ()
{
	delete m_pData;
}

void BinaryFileResource::DoCreateFromFile (ID3D11Device & _device)
{
	DoCreateFromBinary (_device, m_pData, m_cData);
}