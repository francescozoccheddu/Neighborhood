#include <Game/Rendering/ShadowingSubPass.hpp>
#include <Game/Utils/Exceptions.hpp>
#include <Game/Utils/COMExceptions.hpp>

ShadowingSubPass::ShadowingSubPass (const GeometryPass & _geometryPass) : m_GeometryPass { _geometryPass }
{
	for (int iMap { 0 }; iMap < s_cDirectionalMaps; iMap++)
	{
		m_pDirectionalMaps[iMap] = new DepthMap2DResource (s_DirectionalSize, s_DirectionalSize);
	}
	for (int iMap { 0 }; iMap < s_cPointMaps; iMap++)
	{
		m_pPointMaps[iMap] = new DepthMap3DResource (s_PointSize);
	}
	for (int iMap { 0 }; iMap < s_cConeMaps; iMap++)
	{
		m_pConeMaps[iMap] = new DepthMap2DResource (s_ConeSize, s_ConeSize);
	}
}

ShadowingSubPass::~ShadowingSubPass ()
{
	for (int iMap { 0 }; iMap < s_cDirectionalMaps; iMap++)
	{
		delete m_pDirectionalMaps[iMap];
	}
	for (int iMap { 0 }; iMap < s_cPointMaps; iMap++)
	{
		delete m_pPointMaps[iMap];
	}
	for (int iMap { 0 }; iMap < s_cConeMaps; iMap++)
	{
		delete m_pConeMaps[iMap];
	}
}

void ShadowingSubPass::Load ()
{}

void ShadowingSubPass::Unload ()
{}

bool ShadowingSubPass::IsLoaded () const
{
	return false;
}

void ShadowingSubPass::Create (ID3D11Device & _device)
{
	for (int iMap { 0 }; iMap < s_cDirectionalMaps; iMap++)
	{
		m_pDirectionalMaps[iMap]->Create (_device);
	}
	for (int iMap { 0 }; iMap < s_cPointMaps; iMap++)
	{
		m_pPointMaps[iMap]->Create (_device);
	}
	for (int iMap { 0 }; iMap < s_cConeMaps; iMap++)
	{
		m_pConeMaps[iMap]->Create (_device);
	}
}

void ShadowingSubPass::Destroy ()
{
	for (int iMap { 0 }; iMap < s_cDirectionalMaps; iMap++)
	{
		m_pDirectionalMaps[iMap]->Destroy ();
	}
	for (int iMap { 0 }; iMap < s_cPointMaps; iMap++)
	{
		m_pPointMaps[iMap]->Destroy ();
	}
	for (int iMap { 0 }; iMap < s_cConeMaps; iMap++)
	{
		m_pConeMaps[iMap]->Destroy ();
	}
}

bool ShadowingSubPass::IsCreated () const
{
	return false;
}

ShadowingSubPass::ProcessOutput ShadowingSubPass::ProcessLights (ID3D11DeviceContext & _context, const std::vector<Scene::Drawable>& _drawables, std::list<const Light*>& _pLights, const ProcessLimits & _limits)
{
	D3D11_VIEWPORT oldViewport;
	{
		UINT cViewports { 1 };
		_context.RSGetViewports (&cViewports, &oldViewport);
	}

	D3D11_VIEWPORT viewport;
	viewport.MaxDepth = 1.0f;
	viewport.MinDepth = 0.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	ProcessOutput output;
	ProcessLimits limits = _limits;
	if (limits.directional.cWithShadow > s_cDirectionalMaps)
	{
		limits.directional.cWithShadow = s_cDirectionalMaps;
	}
	if (limits.cone.cWithShadow > s_cConeMaps)
	{
		limits.cone.cWithShadow = s_cConeMaps;
	}
	if (limits.point.cWithShadow > s_cPointMaps)
	{
		limits.point.cWithShadow = s_cPointMaps;
	}

	std::list<Task> tasks { Prepare (_pLights, output, limits) };
	for (const Task & task : tasks)
	{
		GeometryPass::ConstantBuffer buffer;
		buffer.projView = task.transform;
		viewport.Width = viewport.Height = task.viewportSize;
		_context.RSSetViewports (1, &viewport);
		m_GeometryPass.RenderDepthOnly (_drawables, _context, buffer, task.pTarget);
	}
	_context.RSSetViewports (1, &oldViewport);
	return output;
}

std::list<ShadowingSubPass::Task> ShadowingSubPass::Prepare (std::list<const Light*> & _pLights, ProcessOutput & _output, const ProcessLimits & _limits) const
{
	std::list<Task> tasks;
	int iDirectional { 0 }, iPoint { 0 }, iCone { 0 };
	int iDirectionalShadow { 0 }, iPointShadow { 0 }, iConeShadow { 0 };
	std::list<const Light*>::iterator it { _pLights.begin () };
	while (it != _pLights.end ())
	{
		const Light * pLight = *it;
		bool bProcess { false };
		switch (pLight->GetType ())
		{
			case Light::Type::POINT:
				if (iPoint < _limits.point.cTotal && (!pLight->bCastShadows || iPointShadow < _limits.point.cWithShadow))
				{
					bProcess = true;
					iPoint++;
					const PointLight * pPointLight { reinterpret_cast<const PointLight*>(pLight) };
					ProcessedLight<PointLight> outputLight;
					outputLight.pLight = pPointLight;
					if (pLight->bCastShadows)
					{
						const DepthMap3DResource & map { *m_pPointMaps[iPointShadow] };
						constexpr D3D11_TEXTURECUBE_FACE faces[] {
							D3D11_TEXTURECUBE_FACE_POSITIVE_X,
							D3D11_TEXTURECUBE_FACE_NEGATIVE_X,
							D3D11_TEXTURECUBE_FACE_POSITIVE_Y,
							D3D11_TEXTURECUBE_FACE_NEGATIVE_Y,
							D3D11_TEXTURECUBE_FACE_POSITIVE_Z,
							D3D11_TEXTURECUBE_FACE_NEGATIVE_Z };
						PerspectiveProjection proj { pPointLight->CalcProjection () };
						for (D3D11_TEXTURECUBE_FACE face : faces)
						{
							Task task;
							task.pTarget = map.GetTarget (face);
							task.viewportSize = s_PointSize;
							task.transform = proj * pPointLight->CalcView (face);
							tasks.push_back (task);
						}
						outputLight.pShadowMapShaderResource = map.GetShaderResourceView ();
						outputLight.shadowMapSize = s_PointSize;
						iPointShadow++;
					}
					else
					{
						outputLight.pShadowMapShaderResource = nullptr;
						outputLight.shadowMapSize = 0;
					}
					_output.pointLights.push_back (outputLight);
				}
				break;
			case Light::Type::CONE:
				if (iCone < _limits.cone.cTotal && (!pLight->bCastShadows || iConeShadow < _limits.cone.cWithShadow))
				{
					bProcess = true;
					iCone++;
					const ConeLight * pConeLight { reinterpret_cast<const ConeLight*>(pLight) };
					ProcessedLight<ConeLight> outputLight;
					outputLight.pLight = pConeLight;
					if (pLight->bCastShadows)
					{
						const DepthMap2DResource & map { *m_pConeMaps[iConeShadow] };
						Task task;
						task.transform = *pConeLight;
						task.viewportSize = s_ConeSize;
						task.pTarget = map.GetTarget ();
						tasks.push_back (task);
						outputLight.pShadowMapShaderResource = map.GetShaderResourceView ();
						outputLight.shadowMapSize = s_ConeSize;
						iConeShadow++;
					}
					else
					{
						outputLight.pShadowMapShaderResource = nullptr;
						outputLight.shadowMapSize = 0;
					}
					_output.coneLights.push_back (outputLight);
				}
				break;
			case Light::Type::DIRECTION:
				if (iDirectional < _limits.directional.cTotal && (!pLight->bCastShadows || iDirectionalShadow < _limits.directional.cWithShadow))
				{
					bProcess = true;
					iDirectional++;
					const DirectionalLight * pDirectionalLight { reinterpret_cast<const DirectionalLight*>(pLight) };
					ProcessedLight<DirectionalLight> outputLight;
					outputLight.pLight = pDirectionalLight;
					if (pLight->bCastShadows)
					{
						const DepthMap2DResource & map { *m_pDirectionalMaps[iDirectionalShadow] };
						Task task;
						task.transform = *pDirectionalLight;
						task.viewportSize = s_DirectionalSize;
						task.pTarget = map.GetTarget ();
						tasks.push_back (task);
						outputLight.pShadowMapShaderResource = map.GetShaderResourceView ();
						outputLight.shadowMapSize = s_DirectionalSize;
						iDirectionalShadow++;
					}
					else
					{
						outputLight.pShadowMapShaderResource = nullptr;
						outputLight.shadowMapSize = 0;
					}
					_output.directionalLights.push_back (outputLight);
				}
				break;
			default:
				GAME_THROW_MSG ("Unknown light type");
		}
		if (bProcess)
		{
			it = _pLights.erase (it);
		}
		else
		{
			it++;
		}
	}
	return tasks;
}



