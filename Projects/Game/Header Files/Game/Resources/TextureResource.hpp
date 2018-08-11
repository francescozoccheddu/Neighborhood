#pragma once

#include <Game/Resources/Resource.hpp>
#include <Game/Direct3D.hpp>

class TextureResource : public BinaryFileResource
{

public:

	using BinaryFileResource::BinaryFileResource;

	~TextureResource ();

	void SetShaderResource (ID3D11DeviceContext & deviceContext, UINT slot) const;

	const ID3D11Resource * GetResource () const;

	const ID3D11ShaderResourceView * GetShaderResourceView () const;

protected:

	void DoCreateFromBinary (ID3D11Device & device, const void * pData, int cData);

	void DoDestroy ();

private:

	ID3D11Resource * m_pResource;
	ID3D11ShaderResourceView * m_pResourceView;

};