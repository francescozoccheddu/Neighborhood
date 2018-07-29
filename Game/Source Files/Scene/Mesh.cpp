#include <Game/Scene/Mesh.hpp>
#include <Game/Utils/Exceptions.hpp>
#include <Game/Utils/COMExceptions.hpp>
#include <Game/Utils/Direct3D11.hpp>

void Mesh::SetIAVertexBuffer (ID3D11DeviceContext & _deviceContext, ID3D11Buffer * _pBuffer)
{
	UINT stride{ sizeof(float) * 3 };
	UINT offset{ 0 };
	_deviceContext.IASetVertexBuffers (0, 1, &_pBuffer, &stride, &offset);
}

void Mesh::SetIAIndexBuffer (ID3D11DeviceContext & _deviceContext, ID3D11Buffer * _pBuffer)
{
	_deviceContext.IASetIndexBuffer (_pBuffer, DXGI_FORMAT_R32_UINT, 0);
}

Mesh::Mesh() : m_pVertices{nullptr}, m_cVertices{0}, m_pIndices{nullptr}, m_cIndices{0} {}

Mesh::Mesh (float * _pVertices, int _cVertices, unsigned int * _pIndices, int _cIndices)
	: m_pVertices{ _pVertices }, m_cVertices{ _cVertices }, m_pIndices{ _pIndices }, m_cIndices{ _cIndices }
{
	GAME_ASSERT_MSG (_pVertices && _pIndices, "Vertices and indices pointers cannot be nullptr");
	GAME_ASSERT_MSG (_cVertices >= 0 && _cIndices >= 0, "Vertices and indices count cannot be negative");
}

void Mesh::Set (float * _pVertices, int _cVertices, unsigned int * _pIndices, int _cIndices)
{
	GAME_ASSERT_MSG (_pVertices && _pIndices, "Vertices and indices pointers cannot be nullptr");
	GAME_ASSERT_MSG (_cVertices >= 0 && _cIndices >= 0, "Vertices and indices count cannot be negative");
	m_pVertices = _pVertices;
	m_pIndices = _pIndices;
	m_cVertices = _cVertices;
	m_cIndices = _cIndices;
}

void Mesh::Destroy ()
{
	delete[] m_pVertices;
	delete[] m_pIndices;
	m_pVertices = nullptr;
	m_cVertices = 0;
	m_pIndices = nullptr;
	m_cIndices = 0;
}

ID3D11Buffer * Mesh::CreateD3DVertexBuffer (ID3D11Device & _device) const
{
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = m_cVertices * sizeof (float);
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = sizeof(float) * 3;
	desc.Usage = D3D11_USAGE_DEFAULT;
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = m_pVertices;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;
	ID3D11Buffer * pBuffer;
	GAME_COMC (_device.CreateBuffer (&desc, &data, &pBuffer));
	return pBuffer;
}

ID3D11Buffer * Mesh::CreateD3DIndexBuffer (ID3D11Device & _device) const
{
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	desc.ByteWidth = m_cIndices * sizeof (unsigned int);
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = sizeof (unsigned int);
	desc.Usage = D3D11_USAGE_DEFAULT;
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = m_pIndices;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;
	ID3D11Buffer * pBuffer;
	GAME_COMC (_device.CreateBuffer (&desc, &data, &pBuffer));
	return pBuffer;
}

int Mesh::GetIndicesCount () const
{
	return m_cIndices;
}