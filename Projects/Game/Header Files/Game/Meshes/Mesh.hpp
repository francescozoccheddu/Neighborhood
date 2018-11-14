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

};