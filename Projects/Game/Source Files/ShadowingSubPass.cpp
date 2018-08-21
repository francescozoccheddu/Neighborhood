#include <Game/Rendering/ShadowingSubPass.hpp>
#include <Game/Utils/Exceptions.hpp>
#include <Game/Utils/COMExceptions.hpp>
#include "..\Header Files\Game\Rendering\ShadowingSubPass.hpp"

#define _SIZE 1024

ShadowingSubPass::ShadowingSubPass (const GeometryPass & _geometryPass) : m_GeometryPass { _geometryPass }
{
	for (int iMap { 0 }; iMap < s_cDirectionalMaps; iMap++)
	{
		m_pDirectionalMaps[iMap] = new ShadowMap2DResource (s_DirectionalSize);
	}
	for (int iMap { 0 }; iMap < s_cPointMaps; iMap++)
	{
		m_pPointMaps[iMap] = new ShadowMap3DResource (s_PointSize);
	}
	for (int iMap { 0 }; iMap < s_cConeMaps; iMap++)
	{
		m_pConeMaps[iMap] = new ShadowMap2DResource (s_ConeSize);
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

std::list<ShadowingSubPass::ProcessedLight> ShadowingSubPass::ProcessLights (ID3D11DeviceContext & context, const std::vector<Scene::Drawable>& drawables, std::list<const Light*>& pLights)
{
	GAME_THROW_MSG ("Unimplemented");
}

ShadowingSubPass::Intermediate ShadowingSubPass::Prepare (std::list<const Light*> & _pLights) const
{
	Intermediate intermediate;
	int iDirectional { 0 }, iPoint { 0 }, iCone { 0 };
	std::list<const Light*>::iterator it { _pLights.begin () };
	while (it != _pLights.end ())
	{
		const Light * pLight = *it;
		if (pLight->bCastShadows)
		{
			switch (pLight->GetType ())
			{
				case Light::Type::POINT:
					if (iPoint < s_cPointMaps)
					{
						const ShadowMap3DResource & map { *m_pPointMaps[iPoint++] };
						const PointLight & light { *reinterpret_cast<const PointLight*>(pLight) };
						PerspectiveProjection proj;
						proj.aspectRatio = 1.0f;
						proj.farZ = light.farZ;
						proj.nearZ = light.nearZ;
						proj.vFov = DirectX::XMConvertToRadians (90.0f);
						proj.Update ();
						ViewWithTarget view;
						view.position = pLight->position;
						ProcessedLight processedLight;
						processedLight.pShaderResource = map.GetShaderResourceView ();
						processedLight.pLight = pLight;
						Task task;

						task.pTarget = map.GetTarget (D3D11_TEXTURECUBE_FACE_POSITIVE_X);
						view.target = { 1.0f, 0.0f, 0.0f };
						view.unrotatedUp = { 0.0f, 1.0f, 0.0f };
						processedLight.transform = task.transform = proj * view;
						intermediate.tasks.push_back (task);

						task.pTarget = map.GetTarget (D3D11_TEXTURECUBE_FACE_NEGATIVE_X);
						view.target = { -1.0f, 0.0f, 0.0f };
						view.unrotatedUp = { 0.0f, 1.0f, 0.0f };
						task.transform = proj * view;
						intermediate.tasks.push_back (task);

						task.pTarget = map.GetTarget (D3D11_TEXTURECUBE_FACE_POSITIVE_Y);
						view.target = { 0.0f, 1.0f, 0.0f };
						view.unrotatedUp = { 0.0f, 0.0f, -1.0f };
						task.transform = proj * view;
						intermediate.tasks.push_back (task);

						task.pTarget = map.GetTarget (D3D11_TEXTURECUBE_FACE_NEGATIVE_Y);
						view.target = { 0.0f, -1.0f, 0.0f };
						view.unrotatedUp = { 0.0f, 0.0f, 1.0f };
						task.transform = proj * view;
						intermediate.tasks.push_back (task);

						task.pTarget = map.GetTarget (D3D11_TEXTURECUBE_FACE_POSITIVE_Z);
						view.target = { 0.0f, 0.0f, 1.0f };
						view.unrotatedUp = { 0.0f, 1.0f, 0.0f };
						task.transform = proj * view;
						intermediate.tasks.push_back (task);

						task.pTarget = map.GetTarget (D3D11_TEXTURECUBE_FACE_NEGATIVE_Z);
						view.target = { 0.0f, 0.0f, -1.0f };
						view.unrotatedUp = { 0.0f, 1.0f, 0.0f };
						task.transform = proj * view;
						intermediate.tasks.push_back (task);

						intermediate.lights.push_back (processedLight);
					}
					else if (iDirectional >= s_cDirectionalMaps && iCone >= s_cConeMaps)
					{
						return intermediate;
					}
					break;
				case Light::Type::DIRECTION:
					if (iDirectional < s_cDirectionalMaps)
					{
						const ShadowMap2DResource & map { *m_pDirectionalMaps[iDirectional++] };
						const DirectionalLight & light { *reinterpret_cast<const DirectionalLight*>(pLight) };
						OrthographicProjection proj;
						proj.aspectRatio = 1.0f;
						proj.farZ = light.farZ;
						proj.nearZ = light.nearZ;
						proj.height = light.shadowSize;
						proj.Update ();
						ViewWithDirection view;
						view.position = light.position;
						view.direction = light.direction;
						ProcessedLight processedLight;
						processedLight.pShaderResource = map.GetShaderResourceView ();
						processedLight.pLight = pLight;
						Task task;
						task.pTarget = map.GetTarget ();
						processedLight.transform = task.transform = proj * view;
						intermediate.tasks.push_back (task);
						intermediate.lights.push_back (processedLight);
					}
					else if (iPoint >= s_cPointMaps && iCone >= s_cConeMaps)
					{
						return intermediate;
					}
					break;
				case Light::Type::CONE:
					if (iCone < s_cConeMaps)
					{
						const ShadowMap2DResource & map { *m_pConeMaps[iCone++] };
						const ConeLight & light { *reinterpret_cast<const ConeLight*>(pLight) };
						PerspectiveProjection proj;
						proj.aspectRatio = 1.0f;
						proj.farZ = light.farZ;
						proj.nearZ = light.nearZ;
						proj.vFov = light.outerAngle;
						proj.Update ();
						ViewWithDirection view;
						view.position = light.position;
						view.direction = light.direction;
						ProcessedLight processedLight;
						processedLight.pShaderResource = map.GetShaderResourceView ();
						processedLight.pLight = pLight;
						Task task;
						task.pTarget = map.GetTarget ();
						processedLight.transform = task.transform = proj * view;
						intermediate.tasks.push_back (task);
						intermediate.lights.push_back (processedLight);
					}
					else if (iPoint >= s_cPointMaps && iDirectional >= s_cDirectionalMaps)
					{
						return intermediate;
					}
					break;
				default:
					GAME_THROW_MSG ("Unknown light type");
			}
		}
		else
		{
			ProcessedLight processedLight;
			processedLight.pLight = pLight;
			processedLight.pShaderResource = nullptr;
			intermediate.lights.push_back (processedLight);
		}
	}
	return intermediate;
}


