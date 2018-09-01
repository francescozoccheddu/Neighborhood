#include <Game/Resources/DepthMapResource.hpp>

#include <Game/Utils/Exceptions.hpp>
#include <Game/Utils/COMExceptions.hpp>
#include "..\Header Files\Game\Resources\DepthMapResource.hpp"

DepthMapResource::DepthMapResource (int _width, int _height, bool _bCubic, int _cArray) : m_Width { _width }, m_Height { _height }, m_bCubic { _bCubic }, m_cArray { _cArray }, m_bArray { true }
{
	GAME_ASSERT_MSG (_width > 0 && _height > 0, "Size must be positive");
	GAME_ASSERT_MSG (_cArray > 0, "Array size must be positive");
}

DepthMapResource::DepthMapResource (int _width, int _height, bool _bCubic) : m_Width { _width }, m_Height { _height }, m_bCubic { _bCubic }, m_cArray { 1 }, m_bArray { false }
{
	GAME_ASSERT_MSG (_width > 0 && _height > 0, "Size must be positive");
}

DepthMapResource::~DepthMapResource ()
{
	if (DepthMapResource::IsCreated ())
	{
		DepthMapResource::Destroy ();
	}
}

ID3D11ShaderResourceView * DepthMapResource::GetShaderResourceView () const
{
	return m_pShaderResourceView;
}

ID3D11DepthStencilView * DepthMapResource::GetDepthStencilView () const
{
	return m_pDepthStencilView;
}

void DepthMapResource::Clear (ID3D11DeviceContext & _context, FLOAT _value)
{
	GAME_ASSERT_MSG (IsCreated (), "Not created");
	_context.ClearDepthStencilView (m_pDepthStencilView, D3D11_CLEAR_DEPTH, _value, 0);
}

void DepthMapResource::SetPixelShaderResource (ID3D11DeviceContext & _context, UINT _slot) const
{
	GAME_ASSERT_MSG (IsCreated (), "Not created");
	_context.PSSetShaderResources (_slot, 1, &m_pShaderResourceView);
}

void DepthMapResource::SetDepthStencilView (ID3D11DeviceContext & _context) const
{
	GAME_ASSERT_MSG (IsCreated (), "Not created");
	_context.OMSetRenderTargets (0, nullptr, m_pDepthStencilView);
}

bool DepthMapResource::IsCube () const
{
	return m_bCubic;
}

int DepthMapResource::GetArraySize () const
{
	return m_cArray;
}

bool DepthMapResource::IsArray () const
{
	return m_bArray;
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
	return m_pShaderResourceView != nullptr;
}

void DepthMapResource::Create (ID3D11Device & _device)
{
	GAME_ASSERT_MSG (!IsCreated (), "Already created");
	com_ptr<ID3D11Texture2D> texture;
	{
		D3D11_TEXTURE2D_DESC textureDesc;
		textureDesc.Width = static_cast<UINT>(GetWidth ());
		textureDesc.Height = static_cast<UINT>(GetHeight ());
		textureDesc.ArraySize = m_bCubic ? m_cArray * 6 : m_cArray;
		textureDesc.MipLevels = 1;
		textureDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;
		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = m_bCubic ? D3D11_RESOURCE_MISC_TEXTURECUBE : 0;
		GAME_COMC (_device.CreateTexture2D (&textureDesc, nullptr, texture.put ()));
	}
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC depthViewDesc {};
		depthViewDesc.Flags = 0;
		depthViewDesc.Format = DXGI_FORMAT_D32_FLOAT;
		if (m_bArray)
		{
			depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			depthViewDesc.Texture2DArray.ArraySize = m_bCubic ? m_cArray * 6 : m_cArray;
			depthViewDesc.Texture2DArray.FirstArraySlice = 0;
			depthViewDesc.Texture2DArray.MipSlice = 0;
		}
		else
		{
			depthViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			depthViewDesc.Texture2D.MipSlice = 0;
		}
		GAME_COMC (_device.CreateDepthStencilView (texture.get (), &depthViewDesc, &m_pDepthStencilView));
	}
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceDesc {};
		shaderResourceDesc.Format = DXGI_FORMAT_R32_FLOAT;
		if (m_bArray)
		{
			if (m_bCubic)
			{
				shaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBEARRAY;
				shaderResourceDesc.TextureCubeArray.First2DArrayFace = 0;
				shaderResourceDesc.TextureCubeArray.MipLevels = 1;
				shaderResourceDesc.TextureCubeArray.MostDetailedMip = 0;
				shaderResourceDesc.TextureCubeArray.NumCubes = m_cArray;
			}
			else
			{
				shaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
				shaderResourceDesc.Texture2DArray.ArraySize = m_cArray;
				shaderResourceDesc.Texture2DArray.FirstArraySlice = 0;
				shaderResourceDesc.Texture2DArray.MipLevels = 1;
				shaderResourceDesc.Texture2DArray.MostDetailedMip = 0;
			}
		}
		else
		{
			if (m_bCubic)
			{
				shaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
				shaderResourceDesc.TextureCube.MipLevels = 1;
				shaderResourceDesc.TextureCube.MostDetailedMip = 0;
			}
			else
			{
				shaderResourceDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				shaderResourceDesc.Texture2D.MipLevels = 1;
				shaderResourceDesc.Texture2D.MostDetailedMip = 0;
			}
		}
		GAME_COMC (_device.CreateShaderResourceView (texture.get (), &shaderResourceDesc, &m_pShaderResourceView));
	}
}

void DepthMapResource::Destroy ()
{
	GAME_ASSERT_MSG (IsCreated (), "Not created");
	m_pShaderResourceView->Release ();
	m_pDepthStencilView->Release ();
	m_pShaderResourceView = nullptr;
	m_pDepthStencilView = nullptr;
}


