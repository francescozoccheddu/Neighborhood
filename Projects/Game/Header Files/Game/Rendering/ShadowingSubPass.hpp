#pragma once
/*
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

	static constexpr int s_cConeMaps { 4 };
	static constexpr int s_ConeSize { 512 };
	static constexpr int s_cDirectionalMaps { 4 };
	static constexpr int s_DirectionalSize { 1024 };
	static constexpr int s_cPointMaps { 4 };
	static constexpr int s_PointSize { 1024 };

	~ShadowingSubPass ();

	void Load () override final;

	void Unload () override final;

	bool IsLoaded () const override final;

	void Create (ID3D11Device & device) override final;

	void Destroy () override final;

	bool IsCreated () const override final;

	std::vector<ProcessedLight> ProcessLights (_In_ ID3D11DeviceContext & context, _In_ const std::vector<Scene::Drawable> & drawables, _Inout_ std::list<const Light *> & pLights, _In_ int cMaxLights);

	const DepthMapResource& GetConeMapResource () const;

	const DepthMapResource& GetDirectionalMapResource () const;

	const DepthMapResource& GetPointMapResource () const;


private:

	static constexpr int s_cMaxSlices { std::max ({ s_cConeMaps, s_cDirectionalMaps, s_cPointMaps * 6 }) };

	struct Task
	{
		DepthMapResource * target;
		std::vector<DirectX::XMFLOAT4X4> transforms;
	};

	struct PrepareResult
	{
		std::vector<Task> tasks;
		std::vector<ProcessedLight> lights;
	};

	struct ModelBufferData
	{
		DirectX::XMFLOAT4X4 transform;
	};

	struct ProjViewBufferData
	{
		alignas(16) UINT cSlices;
		alignas(16) DirectX::XMFLOAT4X4 transforms[s_cMaxSlices];

		inline int GetSize () const
		{
			return sizeof (DirectX::XMFLOAT4X4) * cSlices + offsetof (ProjViewBufferData, transforms);
		}

	};

	PrepareResult ShadowingSubPass::Prepare (_Inout_ std::list<const Light*> & lights, _In_ int cMaxLights) const;

	VertexShaderResource m_VertexShader { GAME_VERTEXSHADER_FILENAME ("Shadowing"), MeshResource::s_aGeometryOnlyInputElementDesc, ARRAYSIZE (MeshResource::s_aGeometryOnlyInputElementDesc) };
	GeometryShaderResource m_GeometryShader { GAME_GEOMETRYSHADER_FILENAME ("Shadowing") };

	ConstantBufferStructResource<ModelBufferData> m_ModelBuffer;
	ConstantBufferStructResource<ProjViewBufferData> m_ProjViewBuffer;

	mutable DepthMapResource m_ConeMaps { s_ConeSize, s_ConeSize, false, s_cConeMaps };
	mutable DepthMapResource m_DirectionalMaps { s_DirectionalSize, s_DirectionalSize, false, s_cDirectionalMaps };
	mutable DepthMapResource m_PointMaps { s_PointSize, s_PointSize, true, s_cPointMaps };

};*/