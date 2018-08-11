#pragma once

#include <Game/Resources/Resource.hpp>

class VertexShaderResource final : public BinaryFileResource
{

public:

	using BinaryFileResource::BinaryFileResource;

	~VertexShaderResource ();

	void SetShaderAndInputLayout (ID3D11DeviceContext & deviceContext) const;

	void Create (ID3D11Device & device) override final;

	void Destroy () override final;

	bool IsCreated () const override final;

private:

	ID3D11VertexShader * m_pShader { nullptr };
	ID3D11InputLayout * m_pInputLayout { nullptr };

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

class ShaderPassResource final : public LoadableResource
{

public:

	ShaderPassResource (const std::string& vertexFileName, const std::string& pixelFileName);

	void Load () override final;

	void Unload () override final;

	bool IsLoaded () const override final;

	void Create (ID3D11Device & device) override final;

	void Destroy () override final;

	bool IsCreated () const override final;

	void Set (ID3D11DeviceContext & deviceContext) const;

private:

	VertexShaderResource m_VertexShader;
	PixelShaderResource m_PixelShader;

};
