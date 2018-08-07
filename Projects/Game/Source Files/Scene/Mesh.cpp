#include <Game/Scene/Mesh.hpp>

#include <Game/Utils/Exceptions.hpp>
#include <Game/Utils/COMExceptions.hpp>

void Mesh::SetIAVertexBuffer (ID3D11DeviceContext & _deviceContext, ID3D11Buffer * _pBuffer)
{
	UINT stride { sizeof (Vertex) };
	UINT offset { 0 };
	_deviceContext.IASetVertexBuffers (0, 1, &_pBuffer, &stride, &offset);
}

void Mesh::SetIAIndexBuffer (ID3D11DeviceContext & _deviceContext, ID3D11Buffer * _pBuffer)
{
	_deviceContext.IASetIndexBuffer (_pBuffer, DXGI_FORMAT_R32_UINT, 0);
}

ID3D11InputLayout* Mesh::CreateInputLayout (ID3D11Device & _device, const void * _pShaderBytecode, SIZE_T _bytecodeLen)
{
	D3D11_INPUT_ELEMENT_DESC desc[2];
	desc[0].SemanticName = "POSITION";
	desc[0].SemanticIndex = 0;
	desc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	desc[0].InputSlot = 0;
	desc[0].AlignedByteOffset = 0;
	desc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	desc[0].InstanceDataStepRate = 0;
	desc[1].SemanticName = "NORMAL";
	desc[1].SemanticIndex = 0;
	desc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	desc[1].InputSlot = 0;
	desc[1].AlignedByteOffset = sizeof (DirectX::XMFLOAT3);
	desc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	desc[1].InstanceDataStepRate = 0;
	ID3D11InputLayout * pInputLayout { nullptr };
	GAME_COMC (_device.CreateInputLayout (desc, 2, _pShaderBytecode, _bytecodeLen, &pInputLayout));
	return pInputLayout;
}

Mesh::Mesh () : m_pVertices { nullptr }, m_cVertices { 0 }, m_pIndices { nullptr }, m_cIndices { 0 } {}

Mesh::Mesh (Vertex * _pVertices, int _cVertices, unsigned int * _pIndices, int _cIndices)
	: m_pVertices { _pVertices }, m_cVertices { _cVertices }, m_pIndices { _pIndices }, m_cIndices { _cIndices }
{
	GAME_ASSERT_MSG (_pVertices && _pIndices, "Vertices and indices pointers cannot be nullptr");
	GAME_ASSERT_MSG (_cVertices >= 0 && _cIndices >= 0, "Vertices and indices count cannot be negative");
}

void Mesh::Set (Vertex * _pVertices, int _cVertices, unsigned int * _pIndices, int _cIndices)
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
	desc.ByteWidth = m_cVertices * sizeof (Vertex);
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = sizeof (Vertex);
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