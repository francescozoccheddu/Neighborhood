#include <Game/Resources/SceneResources.hpp>

#define _DO(_what) \
m_MapMeshResource. _what ; \
m_MapTextureResource. _what ;

void SceneResources::DoCreate (ID3D11Device & _device)
{
	m_pDevice = &_device;
	_DO (CreateAll (_device));
}

void SceneResources::DoDestroy ()
{
	_DO (DestroyAll ());
}

void SceneResources::DestroyUnusedResources (moment_t _maxRestMoments)
{
	moment_t moment { m_Moment - _maxRestMoments };
	_DO (DestroyOlder (moment));
}

void SceneResources::Tick ()
{
	m_Moment++;
}