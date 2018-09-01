#pragma once

#include <Game/Direct3D.hpp>
#include <Game/Scene/Scene.hpp>
#include <Game/Rendering/RenderingPass.hpp>
#include <Game/Resources/ShaderResource.hpp>
#include <Game/Resources/MeshResource.hpp>
#include <Game/Rendering/GeometryPass.hpp>
#include <Game/Resources/DepthMapResource.hpp>
#include <Game/DirectXMath.hpp>
#include <list>
#include <vector>

class ShadowingSubPass final : public RenderingPass
{

public:

	struct ProcessedLight
	{
		const Light * pLight;
		UINT iShadowMap1Based;
	};

	~ShadowingSubPass ();

	void Load () override final;

	void Unload () override final;

	bool IsLoaded () const override final;

	void Create (ID3D11Device & device) override final;

	void Destroy () override final;

	bool IsCreated () const override final;

	std::vector<ProcessedLight> ProcessLights (_In_ ID3D11DeviceContext & context, const _In_ GeometryPass & geometryPass, _In_ const std::vector<Scene::Drawable> & drawables, _Inout_ std::list<const Light *> & pLights, _In_ int cMaxLights);

	const DepthMapResource& GetConeMapResource () const;

	const DepthMapResource& GetDirectionalMapResource () const;

	const DepthMapResource& GetPointMapResource () const;

private:

	struct Task
	{
		DepthMapResource * target;
		std::vector<DirectX::XMFLOAT4X4> transform;
	};

	std::vector<Task> ShadowingSubPass::Prepare (_Inout_ std::list<const Light*> & lights, _Inout_ std::vector<ProcessedLight> & processedLights, _In_ int cMaxLights) const;

	static constexpr int s_cConeMaps { 4 };
	static constexpr int s_ConeSize { 512 };
	static constexpr int s_cDirectionalMaps { 4 };
	static constexpr int s_DirectionalSize { 1024 };
	static constexpr int s_cPointMaps { 4 };
	static constexpr int s_PointSize { 1024 };

	mutable DepthMapResource m_ConeMaps { s_ConeSize, s_ConeSize, false, s_cConeMaps };
	mutable DepthMapResource m_DirectionalMaps { s_DirectionalSize, s_DirectionalSize, false, s_cDirectionalMaps };
	mutable DepthMapResource m_PointMaps { s_PointSize, s_PointSize, true, s_cPointMaps };

};