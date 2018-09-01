#include <Game/Rendering/ShadowingSubPass.hpp>
#include <Game/Utils/Exceptions.hpp>
#include <Game/Utils/COMExceptions.hpp>
#include "..\Header Files\Game\Rendering\ShadowingSubPass.hpp"

ShadowingSubPass::~ShadowingSubPass ()
{
	if (ShadowingSubPass::IsCreated ())
	{
		ShadowingSubPass::Destroy ();
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
	m_ConeMaps.Create (_device);
	m_DirectionalMaps.Create (_device);
	m_PointMaps.Create (_device);
}

void ShadowingSubPass::Destroy ()
{
	m_ConeMaps.Destroy ();
	m_DirectionalMaps.Destroy ();
	m_PointMaps.Destroy ();
}

bool ShadowingSubPass::IsCreated () const
{
	return m_DirectionalMaps.IsCreated ();
}

std::vector<ShadowingSubPass::ProcessedLight> ShadowingSubPass::ProcessLights (ID3D11DeviceContext & _context, const GeometryPass & _geometryPass, const std::vector<Scene::Drawable>& _drawables, std::list<const Light*>& _pLights, int _cMaxLights)
{
	return std::vector<ProcessedLight> ();
}

const DepthMapResource & ShadowingSubPass::GetConeMapResource () const
{
	return m_ConeMaps;
}

const DepthMapResource & ShadowingSubPass::GetDirectionalMapResource () const
{
	return m_DirectionalMaps;
}

const DepthMapResource & ShadowingSubPass::GetPointMapResource () const
{
	return m_PointMaps;
}

std::vector<ShadowingSubPass::Task> ShadowingSubPass::Prepare (std::list<const Light*>& _lights, std::vector<ProcessedLight>& _processedLights, int _cMaxLights) const
{
	int iConeMap { 0 }, iDirectionalMap { 0 }, iPointMap { 0 }, iLight { 0 };
	Task coneTask, directionalTask, pointTask;
	coneTask.target = &m_ConeMaps;
	directionalTask.target = &m_DirectionalMaps;
	pointTask.target = &m_PointMaps;

	auto it { _lights.begin () };

	while (it != _lights.end () && iLight < _cMaxLights)
	{
		const Light & light { **it };
		bool doProcess { true };
		UINT iShadowMap1Based { 0 };

		if (light.bCastShadows)
		{
			switch (light.GetType ())
			{
				case Light::Type::POINT:
					if (iPointMap < s_cPointMaps)
					{
						iShadowMap1Based = ++iPointMap;
						D3D11_TEXTURECUBE_FACE aFaces[] {
							D3D11_TEXTURECUBE_FACE_POSITIVE_X,
							D3D11_TEXTURECUBE_FACE_NEGATIVE_X,
							D3D11_TEXTURECUBE_FACE_POSITIVE_Y,
							D3D11_TEXTURECUBE_FACE_NEGATIVE_Y,
							D3D11_TEXTURECUBE_FACE_POSITIVE_Z,
							D3D11_TEXTURECUBE_FACE_NEGATIVE_Z,
						};
						const PointLight & pointLight { static_cast<const PointLight&>(light) };
						for (auto face : aFaces)
						{
							pointTask.transform.push_back (light * pointLight.CalcView (face));
						}
					}
					else if (s_cPointMaps > 0)
					{
						doProcess = false;
					}
					break;
				case Light::Type::DIRECTION:
					if (iDirectionalMap < s_cDirectionalMaps)
					{
						iShadowMap1Based = ++iDirectionalMap;
						directionalTask.transform.push_back (light);
					}
					else if (s_cDirectionalMaps > 0)
					{
						doProcess = false;
					}
					break;
				case Light::Type::CONE:
					if (iConeMap < s_cConeMaps)
					{
						iShadowMap1Based = ++iConeMap;
						coneTask.transform.push_back (light);
					}
					else if (s_cConeMaps > 0)
					{
						doProcess = false;
					}
					break;
				default:
					break;
			}
		}

		if (doProcess)
		{
			ProcessedLight processedLight;
			processedLight.iShadowMap1Based = iShadowMap1Based;
			processedLight.pLight = &light;
			_processedLights.push_back (processedLight);
			iLight++;
			it = _lights.erase (it);
		}
		else
		{
			it++;
		}

	}

	return std::vector { coneTask, directionalTask, pointTask };
}
