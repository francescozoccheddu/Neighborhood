#pragma once

#include <Game/Resources/Resource.hpp>
#include <vector>

class ConstantBufferResource : public Resource
{

public:

	static void SetForVertexShader (ID3D11DeviceContext & deviceContext, int startingSlot, std::vector<ConstantBufferResource*> buffers);

	static void SetForPixelShader (ID3D11DeviceContext & deviceContext, int startingSlot, std::vector<ConstantBufferResource*> buffers);

	~ConstantBufferResource ();

	void Update (ID3D11DeviceContext & deviceContext) const;

	void SetForVertexShader (ID3D11DeviceContext & deviceContext, int slot) const;

	void SetForPixelShader (ID3D11DeviceContext & deviceContext, int slot) const;

	void Create (ID3D11Device & device) override final;

	void Destroy () override final;

	bool IsCreated () const override final;

protected:

	virtual const void * GetData () const = 0;

	virtual int GetSize () const = 0;

private:

	ID3D11Buffer * m_pBuffer { nullptr };

};

template<typename T>
class ConstantBufferStructResource final : public ConstantBufferResource
{

public:

	T data;

protected:

	const void * GetData () const override final
	{
		return &data;
	}

	int GetSize () const override final
	{
		return sizeof (data);
	}
};