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

	template <typename T>
	struct ProcessedLight
	{
		const T * pLight;
		ID3D11ShaderResourceView * pShadowMapShaderResource;
		int shadowMapSize;
	};

	struct ProcessOutput
	{
		template<typename T>
		using list_t = std::list<ProcessedLight<T>>;

		list_t<ConeLight> coneLights;
		list_t<DirectionalLight> directionalLights;
		list_t<PointLight> pointLights;
	};

	struct ProcessLimits
	{
		struct LightType
		{
			int cTotal;
			int cWithShadow;
		};
		LightType directional;
		LightType point;
		LightType cone;
	};

	ShadowingSubPass (const GeometryPass & geometryPass);

	~ShadowingSubPass ();

	void Load () override final;

	void Unload () override final;

	bool IsLoaded () const override final;

	void Create (ID3D11Device & device) override final;

	void Destroy () override final;

	bool IsCreated () const override final;

	ProcessOutput ProcessLights (ID3D11DeviceContext & context, const std::vector<Scene::Drawable> & drawables, std::list<const Light *> & pLights, const ProcessLimits & limits);

private:

	struct Task
	{
		ID3D11DepthStencilView * pTarget;
		int viewportSize;
		DirectX::XMFLOAT4X4 transform;
	};

	std::list<Task> ShadowingSubPass::Prepare (std::list<const Light*> & pLights, ProcessOutput & processedLights, const ProcessLimits & limits) const;

	static constexpr int s_cDirectionalMaps { 4 };
	static constexpr int s_DirectionalSize { 1024 };
	static constexpr int s_cPointMaps { 4 };
	static constexpr int s_PointSize { 1024 };
	static constexpr int s_cConeMaps { 4 };
	static constexpr int s_ConeSize { 512 };

	const GeometryPass & m_GeometryPass;
	DepthMap2DResource * m_pDirectionalMaps[s_cDirectionalMaps];
	DepthMap3DResource * m_pPointMaps[s_cPointMaps];
	DepthMap2DResource * m_pConeMaps[s_cConeMaps];

};