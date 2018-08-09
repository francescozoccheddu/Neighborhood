#pragma once

#include <Game/Resources/FileResource.hpp>
#include <Game/Direct3D.hpp>
#include <Game/DirectXMath.hpp>
#include <cstdint>

#define GAME_MESHRESOURCE_HALF_INDEX 1

class MeshResource : public FileResource
{

public:

	using FileResource::FileResource;

	inline constexpr static const D3D11_INPUT_ELEMENT_DESC s_aInputElementDesc[] {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXTCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 } };

	static void SetIAVertexBuffer (ID3D11DeviceContext & deviceContext, ID3D11Buffer * pBuffer);

	static void SetIAIndexBuffer (ID3D11DeviceContext & deviceContext, ID3D11Buffer * pBuffer);

	void SetBuffers (ID3D11DeviceContext & deviceContext) const;

	ID3D11Buffer * GetVertexBuffer () const;

	ID3D11Buffer * GetIndexBuffer () const;

	int GetVerticesCount () const;

	int GetIndicesCount () const;

protected:

	void DoLoad () override;

	void DoUnload () override;

	void DoCreate (ID3D11Device & device) override;

	void DoDestroy () override;

private:


#if GAME_MESHRESOURCE_HALF_INDEX
	using ind_t = uint16_t;
#else
	using ind_t = uint32_t;
#endif

	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 textureCoord;
	};

	Vertex * m_pVertices;
	int m_cVertices;

	ind_t * m_pIndices;
	int m_cIndices;

	ID3D11Buffer * m_pVertexBuffer;
	ID3D11Buffer * m_pIndexBuffer;

};