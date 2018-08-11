#include <Game/Resources/MeshResource.hpp>

#include <Game/Utils/Storage.hpp>
#include <rapidjson/document.h>
#include <Game/Utils/Exceptions.hpp>
#include <Game/Utils/COMExceptions.hpp>

MeshResource::~MeshResource ()
{
	MeshResource::DoDestroy ();
}

void MeshResource::SetIAVertexBuffer (ID3D11DeviceContext & _deviceContext, ID3D11Buffer * _pBuffer)
{
	UINT stride { sizeof (Vertex) };
	UINT offset { 0 };
	GAME_COMC (_deviceContext.IASetVertexBuffers (0, 1, &_pBuffer, &stride, &offset));
}

void MeshResource::SetIAIndexBuffer (ID3D11DeviceContext & _deviceContext, ID3D11Buffer * _pBuffer)
{
#if GAME_MESHRESOURCE_HALF_INDEX
	GAME_COMC (_deviceContext.IASetIndexBuffer (_pBuffer, DXGI_FORMAT_R16_UINT, 0));
#else
	GAME_COMC (_deviceContext.IASetIndexBuffer (_pBuffer, DXGI_FORMAT_R32_UINT, 0));
#endif
}

void MeshResource::SetBuffers (ID3D11DeviceContext & _deviceContext) const
{
	AssertCreated ();
	SetIAIndexBuffer (_deviceContext, m_pIndexBuffer);
	SetIAVertexBuffer (_deviceContext, m_pVertexBuffer);
}

ID3D11Buffer * MeshResource::GetVertexBuffer () const
{
	AssertCreated ();
	return m_pVertexBuffer;
}

ID3D11Buffer * MeshResource::GetIndexBuffer () const
{
	AssertCreated ();
	return m_pIndexBuffer;
}

int MeshResource::GetVerticesCount () const
{
	return m_cVertices;
}

int MeshResource::GetIndicesCount () const
{
	return m_cIndices;
}

void MeshResource::DoLoad ()
{
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

void MeshResource::DoCreateFromFile (ID3D11Device & _device)
{
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

void MeshResource::DoUnload ()
{
	delete m_pVertices;
	delete m_pIndices;
}

void MeshResource::DoDestroy ()
{
	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release ();
	}
	if (m_pIndexBuffer)
	{
		m_pIndexBuffer->Release ();
	}
}