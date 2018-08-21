#include <Game/Rendering/ShadowingSubPass.hpp>
#include <Game/Utils/Exceptions.hpp>
#include <Game/Utils/COMExceptions.hpp>

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

void ShadowingSubPass::Render (ID3D11DeviceContext & _context, const std::vector<Scene::Drawable> _drawables, const DirectionalLight * _pLights, int _cMaxLights, int _cMaxMaps, const ID3D11ShaderResourceView ** _out_pMaps, int & _out_cLights, int & _out_cMaps)
{
	D3D11_VIEWPORT viewport;
	UINT cViewports = 1;
	_context.RSGetViewports (&cViewports, &viewport);
	D3D11_VIEWPORT newViewport;
	newViewport.Height = _SIZE;
	newViewport.Width = _SIZE;
	newViewport.MaxDepth = 1.0f;
	newViewport.MinDepth = 0.0f;
	newViewport.TopLeftX = 0.0f;
	newViewport.TopLeftY = 0.0f;
	_context.RSSetViewports (1, &newViewport);
	int iLight { 0 }, iMap { 0 };
	while (iLight < _cMaxLights && iMap < _cMaxMaps)
	{
		const DirectionalLight & light { _pLights[iLight] };
		GeometryPass::ConstantBuffer buffer;
		ViewWithTarget view;
		view.position = { 0.0f, 0.0f, -2.0f };
		view.target = { 0.0f, 0.0f, 0.0f };
		view.Update ();
		OrthographicProjection proj;
		proj.aspectRatio = 1.0f;
		proj.farZ = 100.0f;
		proj.nearZ = 0.1f;
		proj.Update ();
		buffer.projView = proj * view;
		//m_GeometryPass.Render (_drawables, _context, buffer, m_ShadowMaps[iMap].pTarget);
		//_out_pMaps[iMap] = m_ShadowMaps[iMap].pShaderResource;
		iLight++;
		iMap++;
	}
	_out_cLights = iLight;
	_out_cMaps = iMap;
	_context.RSSetViewports (1, &viewport);
}


