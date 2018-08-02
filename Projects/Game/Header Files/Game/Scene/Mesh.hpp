#pragma once

struct ID3D11Buffer;
struct ID3D11Device;
struct ID3D11DeviceContext;

class Mesh
{

public:

	static void SetIAVertexBuffer (ID3D11DeviceContext & deviceContext, ID3D11Buffer * pBuffer);

	static void SetIAIndexBuffer (ID3D11DeviceContext & deviceContext, ID3D11Buffer * pBuffer);

	Mesh ();

	Mesh (float * pVertices, int cVertices, unsigned int * pIndices, int cIndices);

	void Set (float * pVertices, int cVertices, unsigned int * pIndices, int cIndices);

	void Destroy ();

	ID3D11Buffer * CreateD3DVertexBuffer (ID3D11Device & device) const;

	ID3D11Buffer * CreateD3DIndexBuffer (ID3D11Device & device) const;

	int GetIndicesCount () const;

private:

	float * m_pVertices;
	int m_cVertices;
	unsigned int * m_pIndices;
	int m_cIndices;

};