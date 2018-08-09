#include <Game/Resources/ShaderResource.hpp>

#include <Game/Resources/MeshResource.hpp>
#include <Game/Utils/Exceptions.hpp>
#include <Game/Utils/COMExceptions.hpp>

ShaderResource::ShaderResource (const std::string& _fileName, ShaderType _eType) : BinaryFileResource { _fileName }, m_eType { _eType } {}

void ShaderResource::SetShaderAndInputLayout (ID3D11DeviceContext & _deviceContext) const
{
	AssertCreated ();
	switch (m_eType)
	{
		case ShaderType::PIXEL:
			GAME_COMC (_deviceContext.PSSetShader (reinterpret_cast<ID3D11PixelShader*>(m_pShader), nullptr, 0));
			break;
		case ShaderType::VERTEX:
			GAME_COMC (_deviceContext.VSSetShader (reinterpret_cast<ID3D11VertexShader*>(m_pShader), nullptr, 0));
			GAME_COMC (_deviceContext.IASetInputLayout (m_pInputLayout));
			break;
		default:
			GAME_THROW_MSG ("Unknown shader type");
	}
}

const ID3D11InputLayout * ShaderResource::GetInputLayout () const
{
	AssertCreated ();
	GAME_ASSERT_MSG (m_eType == ShaderType::VERTEX, "Not a vertex shader");
	return m_pInputLayout;
}

const ID3D11PixelShader * ShaderResource::GetPixelShader () const
{
	AssertCreated ();
	GAME_ASSERT_MSG (m_eType == ShaderType::PIXEL, "Not a pixel shader");
	return reinterpret_cast<ID3D11PixelShader*>(m_pShader);
}

const ID3D11VertexShader * ShaderResource::GetVertexShader () const
{
	AssertCreated ();
	GAME_ASSERT_MSG (m_eType == ShaderType::VERTEX, "Not a vertex shader");
	return reinterpret_cast<ID3D11VertexShader*>(m_pShader);
}

void ShaderResource::DoCreateFromBinary (ID3D11Device & _device, const void * _pData, int _cData)
{
	switch (m_eType)
	{
		case ShaderType::PIXEL:
			GAME_COMC (_device.CreatePixelShader (_pData, static_cast<SIZE_T>(_cData), nullptr, reinterpret_cast<ID3D11PixelShader**>(&m_pShader)));
			break;
		case ShaderType::VERTEX:
			GAME_COMC (_device.CreateVertexShader (_pData, static_cast<SIZE_T>(_cData), nullptr, reinterpret_cast<ID3D11VertexShader**>(&m_pShader)));
			GAME_COMC (_device.CreateInputLayout (MeshResource::s_aInputElementDesc, ARRAYSIZE (MeshResource::s_aInputElementDesc), _pData, static_cast<SIZE_T>(_cData), &m_pInputLayout));
			break;
		default:
			GAME_THROW_MSG ("Unknown shader type");
	}
}

void ShaderResource::DoDestroy ()
{
	reinterpret_cast<IUnknown*>(m_pShader)->Release ();
	if (m_eType == ShaderType::VERTEX)
	{
		m_pInputLayout->Release ();
	}
}