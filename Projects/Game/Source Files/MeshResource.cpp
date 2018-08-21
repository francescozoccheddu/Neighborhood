#include <Game/Resources/MeshResource.hpp>

#include <Game/Utils/Storage.hpp>
#include <rapidjson/document.h>
#include <Game/Utils/Exceptions.hpp>
#include <Game/Utils/COMExceptions.hpp>

SceneMeshResource::~SceneMeshResource ()
{
	if (SceneMeshResource::IsCreated ())
	{
		SceneMeshResource::Destroy ();
	}
	if (SceneMeshResource::IsLoaded ())
	{
		SceneMeshResource::Unload ();
	}
}

void SceneMeshResource::SetBuffers (ID3D11DeviceContext & _deviceContext, bool _bGeometryOnly) const
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

int SceneMeshResource::GetIndicesCount () const
{
	GAME_ASSERT_MSG (IsLoaded (), "Not loaded");
	return m_cIndices;
}

void SceneMeshResource::Load ()
{
	GAME_ASSERT_MSG (!IsLoaded (), "Already loaded");
	rapidjson::Document jDoc;
	jDoc.Parse (Storage::LoadTextFile (GetFileName ()).c_str ());
	{
		const rapidjson::Value& jVerts { jDoc["vertices"] };
		const rapidjson::Value& jPositions { jVerts["positions"] };
		const rapidjson::Value& jNormals { jVerts["normals"] };
		const rapidjson::Value& jTexCoords { jVerts["texcoords"] };
		const rapidjson::SizeType cArr { jPositions.Size () };
		m_cVertices = static_cast<int>(cArr);
		m_pGeometryVertices = new GeometryVertex[m_cVertices];
		m_pShadingVertices = new ShadingVertex[m_cVertices];
		for (rapidjson::SizeType iArr { 0 }; iArr < cArr; iArr++)
		{
			{
				GeometryVertex& geomVert { m_pGeometryVertices[static_cast<unsigned int>(iArr)] };
				const rapidjson::Value & jPos { jPositions[iArr] };
				geomVert.position.x = jPos[0].GetFloat ();
				geomVert.position.y = jPos[1].GetFloat ();
				geomVert.position.z = jPos[2].GetFloat ();
			}
			{
				ShadingVertex& shadVert { m_pShadingVertices[static_cast<unsigned int>(iArr)] };
				const rapidjson::Value & jNorm { jNormals[iArr] };
				const rapidjson::Value & jTexCoord { jTexCoords[iArr] };
				shadVert.normal.x = jNorm[0].GetFloat ();
				shadVert.normal.y = jNorm[1].GetFloat ();
				shadVert.normal.z = jNorm[2].GetFloat ();
				shadVert.textureCoord.x = jTexCoord[0].GetFloat ();
				shadVert.textureCoord.y = jTexCoord[1].GetFloat ();
			}
		}
	}
	{
		const rapidjson::Value& jInds { jDoc["indices"] };
		const rapidjson::SizeType cArr { jInds.Size () };
		m_cIndices = static_cast<int>(cArr);
		m_pIndices = new ind_t[m_cIndices];
		for (rapidjson::SizeType iArr { 0 }; iArr < cArr; iArr++)
		{
			m_pIndices[static_cast<unsigned int>(iArr)] = static_cast<ind_t>(jInds[iArr].GetUint ());
		}
	}
}

void SceneMeshResource::Unload ()
{
	GAME_ASSERT_MSG (SceneMeshResource::IsLoaded (), "Not loaded");
	delete m_pGeometryVertices;
	delete m_pShadingVertices;
	delete m_pIndices;
	m_pGeometryVertices = nullptr;
	m_pShadingVertices = nullptr;
	m_pIndices = nullptr;
}

bool SceneMeshResource::IsLoaded () const
{
	return m_pIndices != nullptr;
}

void SceneMeshResource::Create (ID3D11Device & _device)
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

void SceneMeshResource::Destroy ()
{
	GAME_ASSERT_MSG (SceneMeshResource::IsCreated (), "Not created");
	m_pGeometryVertexBuffer->Release ();
	m_pShadingVertexBuffer->Release ();
	m_pIndexBuffer->Release ();
	m_pGeometryVertexBuffer = nullptr;
	m_pShadingVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
}

bool SceneMeshResource::IsCreated () const
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