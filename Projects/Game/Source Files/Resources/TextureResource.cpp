#include <Game/Resources/TextureResource.hpp>

#include <Game/Utils/COMExceptions.hpp>
#include <Game/DDSLoader.hpp>

TextureResource::~TextureResource ()
{
	TextureResource::DoDestroy ();
}

void TextureResource::SetShaderResource (ID3D11DeviceContext & _deviceContext, UINT _slot) const
{
	AssertCreated ();
	GAME_COMC (_deviceContext.PSSetShaderResources (_slot, 1, &m_pResourceView));
}

const ID3D11Resource * TextureResource::GetResource () const
{
	AssertCreated ();
	return m_pResource;
}

const ID3D11ShaderResourceView * TextureResource::GetShaderResourceView () const
{
	AssertCreated ();
	return m_pResourceView;
}

void TextureResource::DoCreateFromBinary (ID3D11Device & _device, const void * _pData, int _cData)
{
	GAME_COMC (DirectX::CreateDDSTextureFromMemory (&_device, reinterpret_cast<const uint8_t*>(_pData), static_cast<size_t>(_cData), &m_pResource, &m_pResourceView));
}

void TextureResource::DoDestroy ()
{
	if (m_pResource)
	{
		m_pResource->Release ();
	}
	if (m_pResourceView)
	{
		m_pResourceView->Release ();
	}
}