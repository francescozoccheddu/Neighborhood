#pragma once

#include <Game/Direct3D.hpp>
#include <Game/DirectXMath.hpp>

class Mesh
{

public:

	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
	};

	static void SetIAVertexBuffer (ID3D11DeviceContext & deviceContext, ID3D11Buffer * pBuffer);

	static void SetIAIndexBuffer (ID3D11DeviceContext & deviceContext, ID3D11Buffer * pBuffer);

	static ID3D11InputLayout* CreateInputLayout (ID3D11Device & device, const void * pShaderBytecode, SIZE_T bytecodeLen);

	Mesh ();

	Mesh (Vertex * pVertices, int cVertices, unsigned int * pIndices, int cIndices);

	void Set (Vertex * pVertices, int cVertices, unsigned int * pIndices, int cIndices);

	void Destroy ();

	ID3D11Buffer * CreateD3DVertexBuffer (ID3D11Device & device) const;

	ID3D11Buffer * CreateD3DIndexBuffer (ID3D11Device & device) const;

	int GetIndicesCount () const;

private:

	Vertex * m_pVertices;
	int m_cVertices;
	unsigned int * m_pIndices;
	int m_cIndices;

};

