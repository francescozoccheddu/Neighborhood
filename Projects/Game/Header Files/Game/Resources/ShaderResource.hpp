#pragma once

#include <Game/Resources/Resource.hpp>

#define GAME_SHADER_RESOURCES_DIR GAME_RESOURCES_DIR "Shaders/"
#define GAME_SHADER_RESOURCES_EXT ".cso"
#define GAME_SHADER_RESOURCES_FILENAME(_name) (GAME_SHADER_RESOURCES_DIR _name GAME_SHADER_RESOURCES_EXT)
#define GAME_PIXELSHADER_FILENAME_SUFFIX "_pixel"
#define GAME_VERTEXSHADER_FILENAME_SUFFIX "_vertex"
#define GAME_GEOMETRYSHADER_FILENAME_SUFFIX "_geometry"
#define GAME_PIXELSHADER_FILENAME(_passName) GAME_SHADER_RESOURCES_FILENAME(_passName GAME_PIXELSHADER_FILENAME_SUFFIX)
#define GAME_VERTEXSHADER_FILENAME(_passName) GAME_SHADER_RESOURCES_FILENAME(_passName GAME_VERTEXSHADER_FILENAME_SUFFIX)
#define GAME_GEOMETRYSHADER_FILENAME(_passName) GAME_SHADER_RESOURCES_FILENAME(_passName GAME_GEOMETRYSHADER_FILENAME_SUFFIX)

class VertexShaderResource final : public BinaryFileResource
{

public:

	VertexShaderResource (const std::string & fileName, const D3D11_INPUT_ELEMENT_DESC * pDescriptions, int cDescriptions);

	~VertexShaderResource ();

	void SetShaderAndInputLayout (ID3D11DeviceContext & deviceContext) const;

	void Create (ID3D11Device & device) override final;

	void Destroy () override final;

	bool IsCreated () const override final;

private:

	ID3D11VertexShader * m_pShader { nullptr };
	ID3D11InputLayout * m_pInputLayout { nullptr };
	const D3D11_INPUT_ELEMENT_DESC * m_pDescriptions;
	const int m_cDescriptions;

};

class PixelShaderResource final : public BinaryFileResource
{

public:

	using BinaryFileResource::BinaryFileResource;

	~PixelShaderResource ();

	void SetShader (ID3D11DeviceContext & deviceContext) const;

	void Create (ID3D11Device & device) override final;

	void Destroy () override final;

	bool IsCreated () const override final;

private:

	ID3D11PixelShader * m_pShader { nullptr };

};

class GeometryShaderResource final : public BinaryFileResource
{

public:

	using BinaryFileResource::BinaryFileResource;

	~GeometryShaderResource ();

	void SetShader (ID3D11DeviceContext & deviceContext) const;

	void Create (ID3D11Device & device) override final;

	void Destroy () override final;

	bool IsCreated () const override final;

private:

	ID3D11GeometryShader * m_pShader { nullptr };
};