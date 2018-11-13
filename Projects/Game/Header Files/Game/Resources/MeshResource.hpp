#pragma once

#include <Game/Resources/Resource.hpp>
#include <Game/DirectXMath.hpp>
#include <cstdint>

#define GAME_MESHRESOURCE_HALF_INDEX 1

class MeshResource final : public Resource
{

public:

	~MeshResource ();

#if GAME_MESHRESOURCE_HALF_INDEX
	using ind_t = uint16_t;
#else
	using ind_t = uint32_t;
#endif

	struct GeometryVertex
	{
		DirectX::XMFLOAT3 position;
	};

	struct ShadingVertex
	{
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT3 color;
	};

	MeshResource (const GeometryVertex * pGeometry, const ShadingVertex * pShading, const ind_t * pIndices, int cVertices, int cIndices);

	inline constexpr static const D3D11_INPUT_ELEMENT_DESC s_aInputElementDesc[] {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof (MeshResource::GeometryVertex, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, offsetof (MeshResource::ShadingVertex, normal), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 1, offsetof (MeshResource::ShadingVertex, color), D3D11_INPUT_PER_VERTEX_DATA, 0 } };

	inline constexpr static const D3D11_INPUT_ELEMENT_DESC s_aGeometryOnlyInputElementDesc[] {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof (MeshResource::GeometryVertex, position), D3D11_INPUT_PER_VERTEX_DATA, 0 } };

	void SetBuffers (ID3D11DeviceContext & deviceContext, bool bGeometryOnly) const;

	int GetIndicesCount () const;

	void Create (ID3D11Device & device) override final;

	void Destroy () override final;

	bool IsCreated () const override final;

private:

	friend class VertexShaderResource;

	const GeometryVertex * const m_pGeometryVertices;
	const ShadingVertex * const m_pShadingVertices;
	const int m_cVertices;

	const ind_t * const m_pIndices;
	const int m_cIndices;

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