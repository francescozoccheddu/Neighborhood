#include <Game/Resources/ShaderResource.hpp>

#include <Game/Resources/MeshResource.hpp>
#include <Game/Utils/Exceptions.hpp>
#include <Game/Utils/COMExceptions.hpp>
#include "..\Header Files\Game\Resources\ShaderResource.hpp"

VertexShaderResource::VertexShaderResource (const std::string & _fileName, const D3D11_INPUT_ELEMENT_DESC * _pDescs, int _cDescs) :
	BinaryFileResource { _fileName }, m_pDescriptions { _pDescs }, m_cDescriptions { _cDescs }
{}

VertexShaderResource::~VertexShaderResource ()
{
	if (VertexShaderResource::IsCreated ())
	{
		VertexShaderResource::Destroy ();
	}
}

void VertexShaderResource::SetShaderAndInputLayout (ID3D11DeviceContext & _deviceContext) const
{
	GAME_ASSERT_MSG (IsCreated (), "Not created");
	GAME_COMC (_deviceContext.VSSetShader (m_pShader, nullptr, 0));
	GAME_COMC (_deviceContext.IASetInputLayout (m_pInputLayout));
}

void VertexShaderResource::Create (ID3D11Device & _device)
{
	GAME_ASSERT_MSG (!IsCreated (), "Already created");
	GAME_ASSERT_MSG (IsLoaded (), "Not loaded");
	GAME_COMC (_device.CreateVertexShader (GetData (), static_cast<SIZE_T>(GetSize ()), nullptr, &m_pShader));
	GAME_COMC (_device.CreateInputLayout (m_pDescriptions, static_cast<UINT>(m_cDescriptions), GetData (), static_cast<SIZE_T>(GetSize ()), &m_pInputLayout));
}

void VertexShaderResource::Destroy ()
{
	GAME_ASSERT_MSG (IsCreated (), "Not created");
	m_pShader->Release ();
	m_pInputLayout->Release ();
	m_pShader = nullptr;
	m_pInputLayout = nullptr;
}

bool VertexShaderResource::IsCreated () const
{
	return m_pShader != nullptr;
}

PixelShaderResource::~PixelShaderResource ()
{
	if (PixelShaderResource::IsCreated ())
	{
		PixelShaderResource::Destroy ();
	}
}

void PixelShaderResource::SetShader (ID3D11DeviceContext & _deviceContext) const
{
	GAME_ASSERT_MSG (IsCreated (), "Not created");
	GAME_COMC (_deviceContext.PSSetShader (m_pShader, nullptr, 0));
}

void PixelShaderResource::Create (ID3D11Device & _device)
{
	GAME_ASSERT_MSG (!IsCreated (), "Already created");
	GAME_ASSERT_MSG (IsLoaded (), "Not loaded");
	GAME_COMC (_device.CreatePixelShader (GetData (), static_cast<SIZE_T>(GetSize ()), nullptr, &m_pShader));
}

void PixelShaderResource::Destroy ()
{
	GAME_ASSERT_MSG (IsCreated (), "Not created");
	m_pShader->Release ();
	m_pShader = nullptr;
}

bool PixelShaderResource::IsCreated () const
{
	return m_pShader != nullptr;
}

ShaderPassResource::ShaderPassResource (const std::string& _vertexFn, const std::string& _pixelFn, const D3D11_INPUT_ELEMENT_DESC * _pDescs, int _cDescs) : m_VertexShader { _vertexFn, _pDescs, _cDescs }, m_PixelShader { _pixelFn } {}

void ShaderPassResource::Load ()
{
	m_VertexShader.Load ();
	m_PixelShader.Load ();
}

void ShaderPassResource::Unload ()
{
	m_VertexShader.Unload ();
	m_PixelShader.Unload ();
}

bool ShaderPassResource::IsLoaded () const
{
	return m_VertexShader.IsLoaded ();
}

void ShaderPassResource::Create (ID3D11Device & _device)
{
	m_VertexShader.Create (_device);
	m_PixelShader.Create (_device);
}

void ShaderPassResource::Destroy ()
{
	m_VertexShader.Destroy ();
	m_PixelShader.Destroy ();
}

bool ShaderPassResource::IsCreated () const
{
	return m_VertexShader.IsCreated ();
}

void ShaderPassResource::Set (ID3D11DeviceContext & _deviceContext) const
{
	m_VertexShader.SetShaderAndInputLayout (_deviceContext);
	m_PixelShader.SetShader (_deviceContext);
}

void ShaderPassResource::SetVertexOnly (ID3D11DeviceContext & _deviceContext) const
{
	m_VertexShader.SetShaderAndInputLayout (_deviceContext);
}

void ShaderPassResource::SetPixelOnly (ID3D11DeviceContext & _deviceContext) const
{
	m_PixelShader.SetShader (_deviceContext);
}
