#pragma once

#include <Game/Resources/Resource.hpp>
#include <Game/Direct3D.hpp>
#include <Game/DirectXMath.hpp>

class DepthMapResource : public Resource
{

public:

	DepthMapResource (int width, int height);

	ID3D11ShaderResourceView * GetShaderResourceView () const;

	int GetWidth () const;

	int GetHeight () const;

	bool IsCreated () const override final;

protected:

	ID3D11ShaderResourceView * m_pShaderResource { nullptr };

private:

	const int m_Width;
	const int m_Height;

};

class DepthMap2DResource final : public DepthMapResource
{

public:

	~DepthMap2DResource ();

	using DepthMapResource::DepthMapResource;

	ID3D11DepthStencilView * GetTarget () const;

	void Create (ID3D11Device & device) override final;

	void Destroy () override final;

private:

	ID3D11DepthStencilView * m_pTarget { nullptr };

};

class DepthMap3DResource final : public DepthMapResource
{

public:

	~DepthMap3DResource ();

	DepthMap3DResource (int size);

	ID3D11DepthStencilView * GetTarget (D3D11_TEXTURECUBE_FACE face) const;

	void Create (ID3D11Device & device) override final;

	void Destroy () override final;


private:

	ID3D11DepthStencilView * m_pTarget[6] {};

};

