#pragma once

#include <Game/Resources/Resource.hpp>
#include <Game/DirectXMath.hpp>
#include <cstdint>

#define GAME_MESHRESOURCE_HALF_INDEX 1

class SceneMeshResource final : public FileResource
{

public:

	struct Vertex
	{
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT2 textureCoord;
	};

	inline constexpr static const D3D11_INPUT_ELEMENT_DESC s_aInputElementDesc[] {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof (SceneMeshResource::Vertex, position), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof (SceneMeshResource::Vertex, normal), D3D11_INPUT_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, offsetof (SceneMeshResource::Vertex, textureCoord), D3D11_INPUT_PER_VERTEX_DATA, 0 } };

	using FileResource::FileResource;

	~SceneMeshResource ();

	void SetBuffers (ID3D11DeviceContext & deviceContext) const;

	int GetVerticesCount () const;

	int GetIndicesCount () const;

	void Load () override final;

	void Unload () override final;

	bool IsLoaded () const override final;

	void Create (ID3D11Device & device) override final;

	void Destroy () override final;

	bool IsCreated () const override final;

private:

	friend class VertexShaderResource;

#if GAME_MESHRESOURCE_HALF_INDEX
	using ind_t = uint16_t;
#else
	using ind_t = uint32_t;
#endif

	Vertex * m_pVertices { nullptr };
	int m_cVertices { 0 };

	ind_t * m_pIndices { nullptr };
	int m_cIndices { 0 };

	ID3D11Buffer * m_pVertexBuffer { nullptr };
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