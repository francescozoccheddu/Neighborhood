#include <Game/Resources/MeshResource.hpp>

#include <Game/Utils/Storage.hpp>
#include <Game/Utils/Exceptions.hpp>
#include <Game/Utils/COMExceptions.hpp>
#include "..\Header Files\Game\Resources\MeshResource.hpp"

MeshResource::~MeshResource ()
{
	if (MeshResource::IsCreated ())
	{
		MeshResource::Destroy ();
	}
}

MeshResource::MeshResource (const GeometryVertex * _pGeometry, const ShadingVertex * _pShading, const ind_t * _pIndices, int _cVertices, int _cIndices)
	: m_pGeometryVertices { _pGeometry }, m_pShadingVertices { _pShading }, m_pIndices { _pIndices }, m_cVertices { _cVertices }, m_cIndices { _cIndices }
{}

void MeshResource::SetBuffers (ID3D11DeviceContext & _deviceContext, bool _bGeometryOnly) const
{
	GAME_ASSERT_MSG (IsCreated (), "Not created");
#if GAME_MESHRESOURCE_HALF_INDEX
	GAME_COMC (_deviceContext.IASetIndexBuffer (m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0));
#else
	GAME_COMC (_deviceContext.IASetIndexBuffer (m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0));
#endif
	UINT offsets[] { 0, 0 };
	UINT strides[] { sizeof (GeometryVertex), sizeof (ShadingVertex) };
	ID3D11Buffer * buffers[] { m_pGeometryVertexBuffer, _bGeometryOnly ? nullptr : m_pShadingVertexBuffer };
	GAME_COMC (_deviceContext.IASetVertexBuffers (0, 2, buffers, strides, offsets));
}

int MeshResource::GetIndicesCount () const
{
	return m_cIndices;
}

void MeshResource::Create (ID3D11Device & _device)
{
	GAME_ASSERT_MSG (!IsCreated (), "Already created");
	{
		D3D11_BUFFER_DESC desc;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.ByteWidth = m_cVertices * sizeof (GeometryVertex);
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = sizeof (GeometryVertex);
		desc.Usage = D3D11_USAGE_DEFAULT;
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = m_pGeometryVertices;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		GAME_COMC (_device.CreateBuffer (&desc, &data, &m_pGeometryVertexBuffer));
	}
	{
		D3D11_BUFFER_DESC desc;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.ByteWidth = m_cVertices * sizeof (ShadingVertex);
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = sizeof (ShadingVertex);
		desc.Usage = D3D11_USAGE_DEFAULT;
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = m_pShadingVertices;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		GAME_COMC (_device.CreateBuffer (&desc, &data, &m_pShadingVertexBuffer));
	}
	{
		D3D11_BUFFER_DESC desc;
		desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		desc.ByteWidth = m_cIndices * sizeof (ind_t);
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = sizeof (ind_t);
		desc.Usage = D3D11_USAGE_DEFAULT;
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = m_pIndices;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		GAME_COMC (_device.CreateBuffer (&desc, &data, &m_pIndexBuffer));
	}
}

void MeshResource::Destroy ()
{
	GAME_ASSERT_MSG (MeshResource::IsCreated (), "Not created");
	m_pGeometryVertexBuffer->Release ();
	m_pShadingVertexBuffer->Release ();
	m_pIndexBuffer->Release ();
	m_pGeometryVertexBuffer = nullptr;
	m_pShadingVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
}

bool MeshResource::IsCreated () const
{
	return m_pIndexBuffer != nullptr;
}

ScreenMeshResource::~ScreenMeshResource ()
{
	if (ScreenMeshResource::IsCreated ())
	{
		ScreenMeshResource::Destroy ();
	}
}

void ScreenMeshResource::SetBuffer (ID3D11DeviceContext & _deviceContext) const
{
	UINT stride { sizeof (Vertex) };
	UINT offset { 0 };
	GAME_COMC (_deviceContext.IASetVertexBuffers (0, 1, &m_pVertexBuffer, &stride, &offset));
}

int ScreenMeshResource::GetVerticesCount ()
{
	return ARRAYSIZE (s_pVertices);
}

void ScreenMeshResource::Create (ID3D11Device & _device)
{
	GAME_ASSERT_MSG (!IsCreated (), "Already created");
	D3D11_BUFFER_DESC desc;
	desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	desc.ByteWidth = ARRAYSIZE (s_pVertices) * sizeof (Vertex);
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = 0;
	desc.StructureByteStride = sizeof (Vertex);
	desc.Usage = D3D11_USAGE_DEFAULT;
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = s_pVertices;
	data.SysMemPitch = 0;
	data.SysMemSlicePitch = 0;
	GAME_COMC (_device.CreateBuffer (&desc, &data, &m_pVertexBuffer));
}

void ScreenMeshResource::Destroy ()
{
	GAME_ASSERT_MSG (IsCreated (), "Not created");
	m_pVertexBuffer->Release ();
	m_pVertexBuffer = nullptr;
}

bool ScreenMeshResource::IsCreated () const
{
	return m_pVertexBuffer != nullptr;
}