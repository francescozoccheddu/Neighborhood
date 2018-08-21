#pragma once

#include <Game/Resources/Resource.hpp>
#include <Game/Direct3D.hpp>
#include <Game/DirectXMath.hpp>

class ShadowMapResource : public Resource
{

public:

	static void SetShaderResources (ID3D11DeviceContext & context, const ShadowMapResource * pShadowMaps, int cShadowMaps, int startingSlot);

	ShadowMapResource (int size);

	int GetSize () const;

	bool IsCreated () const override final;

protected:

	ID3D11ShaderResourceView * m_pShaderResource { nullptr };

private:

	const int m_Size;

};

class ShadowMap2DResource final : public ShadowMapResource
{

public:

	using ShadowMapResource::ShadowMapResource;

	void SetDepthOnlyTarget (ID3D11DeviceContext & context) const;

	void Create (ID3D11Device & device) override final;

	void Destroy () override final;

private:

	ID3D11DepthStencilView * m_pTarget { nullptr };

};

class ShadowMap3DResource final : public ShadowMapResource
{

public:

	using ShadowMapResource::ShadowMapResource;

	void SetDepthOnlyTarget (ID3D11DeviceContext & context, D3D11_TEXTURECUBE_FACE face) const;

	void Create (ID3D11Device & device) override final;

	void Destroy () override final;


private:

	ID3D11DepthStencilView * m_pTarget[6] {};

};

