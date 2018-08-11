#pragma once

#include <Game/Resources/Resource.hpp>
#include <Game/Direct3D.hpp>

class TextureResource final : public BinaryFileResource
{

public:

	using BinaryFileResource::BinaryFileResource;

	~TextureResource ();

	void SetShaderResourceView (ID3D11DeviceContext & deviceContext, UINT slot) const;

	void Create (ID3D11Device & device) override final;

	void Destroy () override final;

	bool IsCreated () const override final;

private:

	ID3D11ShaderResourceView * m_pResourceView { nullptr };

};