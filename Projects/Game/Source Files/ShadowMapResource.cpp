#include <Game/Resources/ShadowMapResource.hpp>

#include <Game/Utils/Exceptions.hpp>
#include <Game/Utils/COMExceptions.hpp>

void ShadowMapResource::SetShaderResources (ID3D11DeviceContext & _context, const ShadowMapResource * _pShadowMaps, int _cShadowMaps, int _startingSlot)
{
	ID3D11ShaderResourceView ** pViews { new ID3D11ShaderResourceView*[_cShadowMaps] };
	_context.PSSetShaderResources (_startingSlot, _cShadowMaps, pViews);
	delete[] pViews;
}

ShadowMapResource::ShadowMapResource (int _size) : m_Size (_size)
{}

int ShadowMapResource::GetSize () const
{
	return m_Size;
}

bool ShadowMapResource::IsCreated () const
{
	return m_pShaderResource != nullptr;
}

void ShadowMap2DResource::SetDepthOnlyTarget (ID3D11DeviceContext & _context) const
{
	GAME_ASSERT_MSG (IsCreated (), "Not created");
	_context.OMSetRenderTargets (0, nullptr, m_pTarget);
}

void ShadowMap2DResource::Create (ID3D11Device & _device)
{
	GAME_ASSERT_MSG (!IsCreated (), "Already created");
	D3D11_TEXTURE2D_DESC textureDesc;
	UINT size { static_cast<UINT>(GetSize ()) };
	textureDesc.Width = size;
	textureDesc.Height = size;
	textureDesc.ArraySize = 1;
	textureDesc.MipLevels = 1;
	textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;
	com_ptr<ID3D11Texture2D> texture;
	GAME_COMC (_device.CreateTexture2D (&textureDesc, nullptr, texture.put ()));
	D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc {};
	depthViewDesc.Flags = 0;
	depthViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthViewDesc.Texture2D.MipSlice = 0;
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc {};
	shaderResourceDesc.Format = DXGI_FORMAT_R32_FLOAT;
	shaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	shaderResourceDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceDesc.Texture2D.MipLevels = 1;
	GAME_COMC (_device.CreateDepthStencilView (texture.get (), &depthViewDesc, &m_pTarget));
	GAME_COMC (_device.CreateShaderResourceView (texture.get (), &shaderResourceDesc, &m_pShaderResource));
}

void ShadowMap2DResource::Destroy ()
{
	GAME_ASSERT_MSG (IsCreated (), "Not created");
	m_pTarget->Release ();
	m_pShaderResource->Release ();
	m_pTarget = nullptr;
	m_pShaderResource = nullptr;
}

void ShadowMap3DResource::SetDepthOnlyTarget (ID3D11DeviceContext & _context, D3D11_TEXTURECUBE_FACE _face) const
{
	GAME_ASSERT_MSG (IsCreated (), "Not created");
	_context.OMSetRenderTargets (0, nullptr, m_pTarget[static_cast<unsigned int>(_face)]);
}

void ShadowMap3DResource::Create (ID3D11Device & _device)
{
	GAME_ASSERT_MSG (!IsCreated (), "Already created");
	D3D11_TEXTURE2D_DESC textureDesc;
	UINT size { static_cast<UINT>(GetSize ()) };
	textureDesc.Width = size;
	textureDesc.Height = size;
	textureDesc.ArraySize = 6;
	textureDesc.MipLevels = 1;
	textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
	com_ptr<ID3D11Texture2D> texture;
	GAME_COMC (_device.CreateTexture2D (&textureDesc, nullptr, texture.put ()));
	D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc {};
	depthViewDesc.Flags = 0;
	depthViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	for (int iFace { 0 }; iFace < 6; iFace++)
	{
		depthViewDesc.Texture2D.MipSlice = static_cast<UINT>(iFace);
		GAME_COMC (_device.CreateDepthStencilView (texture.get (), &depthViewDesc, &m_pTarget[iFace]));
	}
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc {};
	shaderResourceDesc.Format = DXGI_FORMAT_R32_FLOAT;
	shaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	shaderResourceDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceDesc.Texture2D.MipLevels = 1;
	GAME_COMC (_device.CreateShaderResourceView (texture.get (), &shaderResourceDesc, &m_pShaderResource));
}

void ShadowMap3DResource::Destroy ()
{
	GAME_ASSERT_MSG (IsCreated (), "Not created");
	for (int iFace { 0 }; iFace < 6; iFace++)
	{
		m_pTarget[iFace]->Release ();
		m_pTarget[iFace] = nullptr;
	}
	m_pShaderResource->Release ();
	m_pShaderResource = nullptr;
}

