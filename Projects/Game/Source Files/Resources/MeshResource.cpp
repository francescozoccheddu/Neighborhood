#include <Game/Resources/MeshResource.hpp>

#include <Game/Utils/Storage.hpp>
#include <rapidjson/document.h>
#include <Game/Utils/Exceptions.hpp>
#include <Game/Utils/COMExceptions.hpp>

MeshResource::~MeshResource ()
{
	if (MeshResource::IsCreated ())
	{
		MeshResource::Destroy ();
	}
}

void MeshResource::SetBuffers (ID3D11DeviceContext & _deviceContext) const
{
	GAME_ASSERT_MSG (IsCreated (), "Not created");
#if GAME_MESHRESOURCE_HALF_INDEX
	GAME_COMC (_deviceContext.IASetIndexBuffer (m_pIndexBuffer, DXGI_FORMAT_R16_UINT, 0));
#else
	GAME_COMC (_deviceContext.IASetIndexBuffer (m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0));
#endif
	UINT stride { sizeof (Vertex) };
	UINT offset { 0 };
	GAME_COMC (_deviceContext.IASetVertexBuffers (0, 1, &m_pVertexBuffer, &stride, &offset));
}

int MeshResource::GetVerticesCount () const
{
	GAME_ASSERT_MSG (IsLoaded (), "Not loaded");
	return m_cVertices;
}

int MeshResource::GetIndicesCount () const
{
	GAME_ASSERT_MSG (IsLoaded (), "Not loaded");
	return m_cIndices;
}

void MeshResource::Load ()
{
	GAME_ASSERT_MSG (!IsLoaded (), "Already loaded");
	rapidjson::Document jDoc;
	jDoc.Parse (Storage::LoadTextFile (GetFileName ()).c_str ());
	GAME_ASSERT_MSG (jDoc.IsObject (), "Not a JSON object");
	{
		const rapidjson::Value& jVerts { jDoc["verts"] };
		GAME_ASSERT_MSG (jVerts.IsArray (), "Not a JSON array");
		const rapidjson::SizeType cArr { jVerts.Size () };
		m_cVertices = static_cast<int>(cArr);
		m_pVertices = new Vertex[m_cVertices];
		for (rapidjson::SizeType iArr { 0 }; iArr < cArr; iArr++)
		{
			Vertex& vert { m_pVertices[static_cast<unsigned int>(iArr)] };
			const rapidjson::Value & jVert { jVerts[iArr] };
			int iVert { 0 };
			vert.position.x = jVert[iVert++].GetFloat ();
			vert.position.y = jVert[iVert++].GetFloat ();
			vert.position.z = jVert[iVert++].GetFloat ();
			vert.normal.x = jVert[iVert++].GetFloat ();
			vert.normal.y = jVert[iVert++].GetFloat ();
			vert.normal.z = jVert[iVert++].GetFloat ();
			vert.textureCoord.x = jVert[iVert++].GetFloat ();
			vert.textureCoord.y = jVert[iVert++].GetFloat ();
		}
	}
	{
		const rapidjson::Value& jInds { jDoc["inds"] };
		GAME_ASSERT_MSG (jInds.IsArray (), "Not a JSON array");
		const rapidjson::SizeType cArr { jInds.Size () };
		m_cIndices = static_cast<int>(cArr);
		m_pIndices = new ind_t[m_cIndices];
		for (rapidjson::SizeType iArr { 0 }; iArr < cArr; iArr++)
		{
			m_pIndices[static_cast<unsigned int>(iArr)] = static_cast<ind_t>(jInds[iArr].GetUint ());
		}
	}
}

void MeshResource::Unload ()
{
	GAME_ASSERT_MSG (MeshResource::IsLoaded (), "Not loaded");
	delete m_pVertices;
	delete m_pIndices;
	m_pVertices = nullptr;
	m_pIndices = nullptr;
}

bool MeshResource::IsLoaded () const
{
	return m_pVertices != nullptr;
}

void MeshResource::Create (ID3D11Device & _device)
{
	GAME_ASSERT_MSG (!IsCreated (), "Already created");
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
		GAME_COMC (_device.CreateBuffer (&desc, &data, &m_pVertexBuffer));
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
	m_pVertexBuffer->Release ();
	m_pIndexBuffer->Release ();
	m_pVertexBuffer = nullptr;
	m_pIndexBuffer = nullptr;
}

bool MeshResource::IsCreated () const
{
	return m_pVertexBuffer;
}