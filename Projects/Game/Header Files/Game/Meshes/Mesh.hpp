#pragma once

#include <Game/DirectXMath.hpp>
#include <cstdint>
#include <vector>

#define GAME_MESH_HALF_INDEX 1

struct Mesh final
{

	struct GeometryVertex
	{
		DirectX::XMFLOAT3 position;
	};

	struct ShadingVertex
	{
		DirectX::XMFLOAT3 normal;
		DirectX::XMFLOAT3 color;
	};

#if GAME_MESH_HALF_INDEX
	using ind_t = uint16_t;
#else
	using ind_t = uint32_t;
#endif

	const std::vector<ind_t> indices;
	const std::vector<GeometryVertex> geometry;
	const std::vector<ShadingVertex> shading;

	const GeometryVertex * GetGeometry () const
	{
		return geometry.data ();
	}

	const ShadingVertex * GetShading () const
	{
		return shading.data ();
	}

	const ind_t * GetIndices () const
	{
		return indices.data ();
	}

	int GetIndicesCount () const
	{
		return indices.size ();
	}

	size_t GetIndexBufferSize () const
	{
		return indices.size () * sizeof (ind_t);
	}

	size_t GetGeometryBufferSize () const
	{
		return geometry.size () * sizeof (GeometryVertex);
	}

	size_t GetShadingBufferSize () const
	{
		return shading.size () * sizeof (ShadingVertex);
	}

};