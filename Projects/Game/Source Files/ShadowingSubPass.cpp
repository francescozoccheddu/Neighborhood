#include <Game/Rendering/ShadowingSubPass.hpp>
#include <Game/Utils/Exceptions.hpp>
#include <Game/Utils/COMExceptions.hpp>
#include "..\Header Files\Game\Rendering\ShadowingSubPass.hpp"

#define _SIZE 1024

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

std::list<ShadowingSubPass::ProcessedLight> ShadowingSubPass::ProcessLights (ID3D11DeviceContext & _context, const std::vector<Scene::Drawable>& _drawables, std::list<const Light*>& _pLights)
{
	D3D11_VIEWPORT oldViewport;
	{
		UINT cViewports { 1 };
		_context.RSGetViewports (&cViewports, &oldViewport);
	}
	{
		D3D11_VIEWPORT viewport;
		viewport.Height = _SIZE;
		viewport.Width = _SIZE;
		viewport.MaxDepth = 1.0f;
		viewport.MinDepth = 0.0f;
		viewport.TopLeftX = 0.0f;
		viewport.TopLeftY = 0.0f;
		_context.RSSetViewports (1, &viewport);
	}
	std::list<ProcessedLight> processedLights;
	std::list<Task> tasks { Prepare (_pLights, processedLights) };
	for (const Task & task : tasks)
	{
		GeometryPass::ConstantBuffer buffer;
		buffer.projView = task.transform;
		m_GeometryPass.Render (_drawables, _context, buffer, task.pTarget);
	}
	_context.RSSetViewports (1, &oldViewport);
	return processedLights;
}

std::list<ShadowingSubPass::Task> ShadowingSubPass::Prepare (std::list<const Light*> & _pLights, std::list<ProcessedLight> & _processedLights) const
{
	std::list<Task> tasks;
	int iDirectional { 0 }, iPoint { 0 }, iCone { 0 };
	std::list<const Light*>::iterator it { _pLights.begin () };
	while (it != _pLights.end ())
	{
		bool bProcessed { false };
		ID3D11ShaderResourceView * pShadowMapShaderResource { nullptr };
		const Light * pLight = *it;
		if (pLight->bCastShadows)
		{
			switch (pLight->GetType ())
			{
				case Light::Type::POINT:
					pShadowMapShaderResource = PreparePoint (*reinterpret_cast<const PointLight*>(pLight), tasks, iPoint);
					break;
				case Light::Type::DIRECTION:
					pShadowMapShaderResource = PrepareDirectional (*reinterpret_cast<const DirectionalLight*>(pLight), tasks, iDirectional);
					break;
				case Light::Type::CONE:
					pShadowMapShaderResource = PrepareCone (*reinterpret_cast<const ConeLight*>(pLight), tasks, iCone);
					break;
				default:
					GAME_THROW_MSG ("Unknown light type");
			}
			bProcessed = pShadowMapShaderResource != nullptr;
		}
		else
		{
			bProcessed = true;
		}
		if (bProcessed)
		{
			it = _pLights.erase (it);
			ProcessedLight processed;
			processed.pLight = pLight;
			processed.pShadowMapShaderResource = pShadowMapShaderResource;
			_processedLights.push_back (processed);
		}
		else
		{
			it++;
		}
	}
	return tasks;
}

ID3D11ShaderResourceView * ShadowingSubPass::PrepareDirectional (const DirectionalLight & _light, std::list<Task> &  _tasks, int & _iMap) const
{
	if (_iMap >= s_cDirectionalMaps)
	{
		return nullptr;
	}
	const DepthMap2DResource & map { *m_pDirectionalMaps[_iMap++] };
	Task task;
	task.transform = _light;
	task.pTarget = map.GetTarget ();
	_tasks.push_back (task);
	return map.GetShaderResourceView ();
}

ID3D11ShaderResourceView * ShadowingSubPass::PreparePoint (const PointLight & _light, std::list<Task> &  _tasks, int & _iMap) const
{
	if (_iMap >= s_cPointMaps)
	{
		return nullptr;
	}
	const DepthMap3DResource & map { *m_pPointMaps[_iMap++] };
	constexpr D3D11_TEXTURECUBE_FACE faces[] {
		D3D11_TEXTURECUBE_FACE_POSITIVE_X,
		D3D11_TEXTURECUBE_FACE_NEGATIVE_X,
		D3D11_TEXTURECUBE_FACE_POSITIVE_Y,
		D3D11_TEXTURECUBE_FACE_NEGATIVE_Y,
		D3D11_TEXTURECUBE_FACE_POSITIVE_Z,
		D3D11_TEXTURECUBE_FACE_NEGATIVE_Z };
	PerspectiveProjection proj { _light.CalcProjection () };
	for (D3D11_TEXTURECUBE_FACE face : faces)
	{
		Task task;
		task.pTarget = map.GetTarget (face);
		task.transform = _light.CalcView (face);
		_tasks.push_back (task);
	}
	return map.GetShaderResourceView ();
}

ID3D11ShaderResourceView * ShadowingSubPass::PrepareCone (const ConeLight & _light, std::list<Task> &  _tasks, int & _iMap) const
{
	if (_iMap >= s_cConeMaps)
	{
		return nullptr;
	}
	const DepthMap2DResource & map { *m_pConeMaps[_iMap++] };
	Task task;
	task.transform = _light;
	task.pTarget = map.GetTarget ();
	_tasks.push_back (task);
	return map.GetShaderResourceView ();
}



