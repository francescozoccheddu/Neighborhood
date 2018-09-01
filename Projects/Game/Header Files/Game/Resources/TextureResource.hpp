#pragma once

#include <Game/Resources/Resource.hpp>
#include <Game/Direct3D.hpp>

#define GAME_TEXTURERESOURCE_DIR GAME_RESOURCES_DIR "Textures/"
#define GAME_TEXTURERESOURCE_EXT ".dds"
#define GAME_TEXTURERESOURCE_FILENAME(_name) (TEXTURERESOURCES_DIR _name TEXTURERESOURCES_EXT)

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