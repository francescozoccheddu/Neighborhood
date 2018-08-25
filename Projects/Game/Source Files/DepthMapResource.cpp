#include <Game/Resources/DepthMapResource.hpp>

#include <Game/Utils/Exceptions.hpp>
#include <Game/Utils/COMExceptions.hpp>

DepthMapResource::DepthMapResource (int _width, int _height) : m_Width { _width }, m_Height { _height }
{}

ID3D11ShaderResourceView * DepthMapResource::GetShaderResourceView () const
{
	return m_pShaderResource;
}

int DepthMapResource::GetWidth () const
{
	return m_Width;
}

int DepthMapResource::GetHeight () const
{
	return m_Height;
}

bool DepthMapResource::IsCreated () const
{
	return m_pShaderResource != nullptr;
}

DepthMap2DResource::~DepthMap2DResource ()
{
	if (DepthMap2DResource::IsCreated ())
	{
		DepthMap2DResource::Destroy ();
	}
}

ID3D11DepthStencilView * DepthMap2DResource::GetTarget () const
{
	return m_pTarget;
}

void DepthMap2DResource::Create (ID3D11Device & _device)
{
	GAME_ASSERT_MSG (!IsCreated (), "Already created");
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = static_cast<UINT>(GetWidth ());
	textureDesc.Height = static_cast<UINT>(GetHeight ());
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

void DepthMap2DResource::Destroy ()
{
	GAME_ASSERT_MSG (IsCreated (), "Not created");
	m_pTarget->Release ();
	m_pShaderResource->Release ();
	m_pTarget = nullptr;
	m_pShaderResource = nullptr;
}

DepthMap3DResource::~DepthMap3DResource ()
{
	if (DepthMap3DResource::IsCreated ())
	{
		DepthMap3DResource::Destroy ();
	}
}

DepthMap3DResource::DepthMap3DResource (int _size) : DepthMapResource (_size, _size)
{}

ID3D11DepthStencilView * DepthMap3DResource::GetTarget (D3D11_TEXTURECUBE_FACE _face) const
{
	return m_pTarget[static_cast<unsigned int>(_face)];
}


void DepthMap3DResource::Create (ID3D11Device & _device)
{
	GAME_ASSERT_MSG (!IsCreated (), "Already created");
	D3D11_TEXTURE2D_DESC textureDesc;
	textureDesc.Width = static_cast<UINT>(GetWidth ());
	textureDesc.Height = static_cast<UINT>(GetHeight ());
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
	depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
	depthViewDesc.Texture2DArray.ArraySize = 1;
	depthViewDesc.Texture2DArray.MipSlice = 0;
	for (int iFace { 0 }; iFace < 6; iFace++)
	{
		depthViewDesc.Texture2DArray.FirstArraySlice = static_cast<UINT>(iFace);
		GAME_COMC (_device.CreateDepthStencilView (texture.get (), &depthViewDesc, &m_pTarget[iFace]));
	}
	D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc {};
	shaderResourceDesc.Format = DXGI_FORMAT_R32_FLOAT;
	shaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	shaderResourceDesc.Texture2D.MostDetailedMip = 0;
	shaderResourceDesc.Texture2D.MipLevels = 1;
	GAME_COMC (_device.CreateShaderResourceView (texture.get (), &shaderResourceDesc, &m_pShaderResource));
}

void DepthMap3DResource::Destroy ()
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

