#include <Game/Resources/TextureResource.hpp>

#include <Game/Utils/COMExceptions.hpp>
#include <Game/DDSLoader.hpp>

TextureResource::~TextureResource ()
{
	if (TextureResource::IsCreated ())
	{
		TextureResource::Destroy ();
	}
}

void TextureResource::SetShaderResourceView (ID3D11DeviceContext & _deviceContext, UINT _slot) const
{
	GAME_ASSERT_MSG (TextureResource::IsCreated (), "Not created");
	GAME_COMC (_deviceContext.PSSetShaderResources (_slot, 1, &m_pResourceView));
}

void TextureResource::Create (ID3D11Device & _device)
{
	GAME_ASSERT_MSG (IsLoaded (), "Not created");
	GAME_COMC (DirectX::CreateDDSTextureFromMemory (&_device, reinterpret_cast<const uint8_t*> (GetData ()), static_cast<size_t>(GetSize ()), nullptr, &m_pResourceView));
}

void TextureResource::Destroy ()
{
	GAME_ASSERT_MSG (TextureResource::IsCreated (), "Not created");
	m_pResourceView->Release ();
	m_pResourceView = nullptr;
}

bool TextureResource::IsCreated () const
{
	return m_pResourceView != nullptr;
}