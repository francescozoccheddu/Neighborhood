#pragma once

#include <Game/Resources/Resource.hpp>
#include <Game/Direct3D.hpp>

class ShaderResource : public BinaryFileResource
{

public:


	enum class ShaderType
	{
		VERTEX, PIXEL
	};

	ShaderResource (const std::string& fileName, ShaderType eType);

	~ShaderResource ();

	void SetShaderAndInputLayout (ID3D11DeviceContext & deviceContext) const;

	const ID3D11InputLayout * GetInputLayout () const;

	const ID3D11PixelShader * GetPixelShader () const;

	const ID3D11VertexShader * GetVertexShader () const;

protected:

	void DoCreateFromBinary (ID3D11Device & device, const void * pData, int cData);

	void DoDestroy ();

private:

	const ShaderType m_eType;
	IUnknown * m_pShader { nullptr };
	ID3D11InputLayout * m_pInputLayout { nullptr };

};

/*
class ShaderPass : public LoadableResource
{

public:

	struct ShaderDef
	{
		const std::string & name;
		const ShaderResource::ShaderType eType;
	};

	ShaderPass (const std::vector<ShaderDef> & shaders);

	void SetShaderAndInputLayout (ID3D11DeviceContext & deviceContext) const;

protected:

	void DoCreateAfterLoad (ID3D11Device & device);

	void DoDestroy ();

	void DoLoad ();

	void DoUnload ();

private:

	std::vector<ShaderResource> m_Shaders;

};*/
