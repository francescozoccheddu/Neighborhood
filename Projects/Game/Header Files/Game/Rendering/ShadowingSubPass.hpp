#pragma once

#include <Game/Direct3D.hpp>
#include <Game/Scene/Scene.hpp>
#include <Game/Rendering/RenderingPass.hpp>
#include <Game/Resources/ShaderResource.hpp>
#include <Game/Resources/MeshResource.hpp>
#include <Game/Rendering/GeometryPass.hpp>
#include <Game/Resources/ShadowMapResource.hpp>
#include <Game/DirectXMath.hpp>
#include <vector>

class ShadowingSubPass final : public RenderingPass
{

public:

	ShadowingSubPass (const GeometryPass & geometryPass);

	~ShadowingSubPass ();

	void Load () override final;

	void Unload () override final;

	bool IsLoaded () const override final;

	void Create (ID3D11Device & device) override final;

	void Destroy () override final;

	bool IsCreated () const override final;

	void Render (ID3D11DeviceContext & _context, const std::vector<Scene::Drawable> drawables, const DirectionalLight * pLights, int cMaxLights, int cMaxMaps, const ID3D11ShaderResourceView ** out_pMaps, int& out_cLights, int& out_cMaps);

private:

	static constexpr int s_cDirectionalMaps { 4 };
	static constexpr int s_DirectionalSize { 1024 };
	static constexpr int s_cPointMaps { 4 };
	static constexpr int s_PointSize { 256 };
	static constexpr int s_cConeMaps { 4 };
	static constexpr int s_ConeSize { 512 };

	const GeometryPass & m_GeometryPass;
	ShadowMap2DResource * m_pDirectionalMaps[s_cDirectionalMaps];
	ShadowMap3DResource * m_pPointMaps[s_cPointMaps];
	ShadowMap2DResource * m_pConeMaps[s_cConeMaps];

};