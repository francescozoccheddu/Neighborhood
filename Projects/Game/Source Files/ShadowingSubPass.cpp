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
{
	m_VertexShader.Load ();
	m_GeometryShader.Load ();
}

void ShadowingSubPass::Unload ()
{
	m_VertexShader.Unload ();
	m_GeometryShader.Unload ();
}

bool ShadowingSubPass::IsLoaded () const
{
	return m_VertexShader.IsLoaded ();
}

void ShadowingSubPass::Create (ID3D11Device & _device)
{
	m_ConeMaps.Create (_device);
	m_DirectionalMaps.Create (_device);
	m_PointMaps.Create (_device);
	m_VertexShader.Create (_device);
	m_GeometryShader.Create (_device);
	m_ProjViewBuffer.Create (_device);
	m_ModelBuffer.Create (_device);
}

void ShadowingSubPass::Destroy ()
{
	m_ConeMaps.Destroy ();
	m_DirectionalMaps.Destroy ();
	m_PointMaps.Destroy ();
	m_VertexShader.Destroy ();
	m_GeometryShader.Destroy ();
	m_ProjViewBuffer.Destroy ();
	m_ModelBuffer.Destroy ();
}

bool ShadowingSubPass::IsCreated () const
{
	return m_VertexShader.IsCreated ();
}

std::vector<ShadowingSubPass::ProcessedLight> ShadowingSubPass::ProcessLights (ID3D11DeviceContext & _context, const SceneResources & _sceneResources, const std::vector<Scene::Drawable>& _drawables, std::list<const Light*>& _pLights, int _cMaxLights)
{
	PrepareResult prepareResult { Prepare (_pLights, _cMaxLights) };

	m_VertexShader.SetShaderAndInputLayout (_context);
	m_GeometryShader.SetShader (_context);
	m_ProjViewBuffer.SetForGeometryShader (_context, 0);
	m_ModelBuffer.SetForVertexShader (_context, 0);

	D3D11_VIEWPORT viewport;
	viewport.TopLeftX = viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	for (const Task & task : prepareResult.tasks)
	{
		viewport.Width = task.target->GetWidth ();
		viewport.Height = task.target->GetHeight ();

		_context.RSSetViewports (1, &viewport);

		task.target->SetDepthStencilView (_context);
		task.target->Clear (_context, 1.0f);

		for (int iSlice { 0 }; iSlice < task.transforms.size (); iSlice++)
		{
			m_ProjViewBuffer.data.transforms[iSlice] = task.transforms[iSlice];
		}

		m_ProjViewBuffer.data.cSlices = task.transforms.size ();
		m_ProjViewBuffer.Update (_context, m_ProjViewBuffer.data.GetSize ());

		for (const Scene::Drawable & drawable : _drawables)
		{
			const SceneMeshResource & mesh { _sceneResources.GetMesh (drawable.mesh) };
			mesh.SetBuffers (_context, true);

			m_ModelBuffer.data.transform = drawable.transform;
			m_ModelBuffer.Update (_context);

			_context.DrawIndexed (mesh.GetIndicesCount (), 0, 0);
		}
	}

	_context.VSSetShader (nullptr, nullptr, 0);
	_context.GSSetShader (nullptr, nullptr, 0);

	return prepareResult.lights;
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

ShadowingSubPass::PrepareResult ShadowingSubPass::Prepare (std::list<const Light*>& _lights, int _cMaxLights) const
{
	int iConeMap { 0 }, iDirectionalMap { 0 }, iPointMap { 0 }, iLight { 0 };
	Task coneTask, directionalTask, pointTask;
	coneTask.target = &m_ConeMaps;
	directionalTask.target = &m_DirectionalMaps;
	pointTask.target = &m_PointMaps;

	std::vector<ProcessedLight> processedLights;

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
							pointTask.transforms.push_back (light * pointLight.CalcView (face));
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
						directionalTask.transforms.push_back (light);
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
						coneTask.transforms.push_back (light);
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
			processedLights.push_back (processedLight);
			iLight++;
			it = _lights.erase (it);
		}
		else
		{
			it++;
		}
	}

	PrepareResult result;
	result.tasks = { coneTask, directionalTask, pointTask };
	result.lights = std::move (processedLights);
	return result;
}
