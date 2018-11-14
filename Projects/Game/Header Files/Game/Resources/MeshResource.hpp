#pragma once

#include <Game/Resources/Resource.hpp>
#include <Game/Direct3D.hpp>
#include <Game/Meshes/Mesh.hpp>


class MeshResource final : public Resource
{

public:

	~MeshResource ();

	MeshResource (const Mesh& mesh);

	inline constexpr static const D3D11_INPUT_ELEMENT_DESC s_aInputElementDesc[] {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof (Mesh::GeometryVertex, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, offsetof (Mesh::ShadingVertex, normal), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, offsetof (Mesh::ShadingVertex, color), D3D11_INPUT_PER_VERTEX_DATA, 0 } };

	inline constexpr static const D3D11_INPUT_ELEMENT_DESC s_aGeometryOnlyInputElementDesc[] {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof (Mesh::GeometryVertex, position), D3D11_INPUT_PER_VERTEX_DATA, 0 } };

	void SetBuffers (ID3D11DeviceContext & deviceContext, bool bGeometryOnly) const;

	int GetIndicesCount () const;

	void Create (ID3D11Device & device) override final;

	void Destroy () override final;

	bool IsCreated () const override final;

private:

	friend class VertexShaderResource;

	const Mesh& m_Mesh;

	ID3D11Buffer * m_pGeometryVertexBuffer { nullptr };
	ID3D11Buffer * m_pShadingVertexBuffer { nullptr };
	ID3D11Buffer * m_pIndexBuffer { nullptr };

};

class ScreenMeshResource : public Resource
{

public:

	struct Vertex
	{
		DirectX::XMFLOAT2 texCoord;
	};

	inline constexpr static const D3D11_INPUT_ELEMENT_DESC s_aInputElementDesc[] {
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, offsetof (Vertex, texCoord), D3D11_INPUT_PER_VERTEX_DATA, 0 } };

	~ScreenMeshResource ();

	void SetBuffer (ID3D11DeviceContext & deviceContext) const;

	static int GetVerticesCount ();

	void Create (ID3D11Device & device) override final;

	void Destroy () override final;

	bool IsCreated () const override final;

private:

	inline static constexpr Vertex s_pVertices[] {
		{ { 0.0f, 1.0f } }, { { 0.0f, 0.0f } }, { { 1.0f, 0.0f } },
	{ { 1.0f, 0.0f } }, { { 1.0f, 1.0f } }, { { 0.0f, 1.0f } }
	};

	ID3D11Buffer * m_pVertexBuffer { nullptr };

};