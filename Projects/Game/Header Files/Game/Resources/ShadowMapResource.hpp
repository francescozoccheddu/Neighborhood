#pragma once

#include <Game/Resources/Resource.hpp>
#include <Game/Direct3D.hpp>
#include <Game/DirectXMath.hpp>

class ShadowMapResource : public Resource
{

public:

	ShadowMapResource (int size);

	const ID3D11ShaderResourceView * GetShaderResourceView () const;

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

	ID3D11DepthStencilView * GetTarget () const;

	void Create (ID3D11Device & device) override final;

	void Destroy () override final;

private:

	ID3D11DepthStencilView * m_pTarget { nullptr };

};

class ShadowMap3DResource final : public ShadowMapResource
{

public:

	using ShadowMapResource::ShadowMapResource;

	ID3D11DepthStencilView * GetTarget (D3D11_TEXTURECUBE_FACE face) const;

	void Create (ID3D11Device & device) override final;

	void Destroy () override final;


private:

	ID3D11DepthStencilView * m_pTarget[6] {};

};

