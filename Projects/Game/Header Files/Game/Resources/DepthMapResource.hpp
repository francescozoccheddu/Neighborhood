#pragma once

#include <Game/Resources/Resource.hpp>
#include <Game/Direct3D.hpp>
#include <Game/DirectXMath.hpp>

class DepthMapResource final : public Resource
{

public:

	DepthMapResource (int width, int height, bool bCubic, int cArray);

	DepthMapResource (int width, int height, bool bCubic);

	~DepthMapResource ();

	ID3D11ShaderResourceView * GetShaderResourceView () const;

	ID3D11DepthStencilView * GetDepthStencilView () const;

	void SetPixelShaderResource (ID3D11DeviceContext & context, UINT slot) const;

	void SetDepthStencilView (ID3D11DeviceContext & context) const;

	bool IsCube () const;

	int GetArraySize () const;

	int GetWidth () const;

	int GetHeight () const;

	void Create (ID3D11Device & device) override final;

	void Destroy () override final;

	bool IsCreated () const override final;

private:

	ID3D11ShaderResourceView * m_pShaderResourceView { nullptr };
	ID3D11DepthStencilView * m_pDepthStencilView { nullptr };

	const int m_Width;
	const int m_Height;
	const bool m_bCubic;
	const int m_cArray;
	const bool m_bArray;

};

