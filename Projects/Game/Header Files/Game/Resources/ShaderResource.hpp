#pragma once

#include <Game/Resources/BinaryFileResource.hpp>
#include <Game/Direct3D.hpp>

class ShaderResource : public BinaryFileResource
{

public:

	enum class ShaderType
	{
		VERTEX, PIXEL
	};

	ShaderResource (const std::string& fileName, ShaderType eType);

	void SetShaderAndInputLayout (ID3D11DeviceContext & deviceContext) const;

	const ID3D11InputLayout * GetInputLayout () const;

	const ID3D11PixelShader * GetPixelShader () const;

	const ID3D11VertexShader * GetVertexShader () const;

protected:

	void DoCreateFromBinary (ID3D11Device & device, const void * pData, int cData);

	void DoDestroy ();

private:

	const ShaderType m_eType;
	void * m_pShader;
	ID3D11InputLayout * m_pInputLayout;

};