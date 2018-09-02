#pragma once

#include <Game/Resources/Resource.hpp>
#include <Game/Utils/Exceptions.hpp>
#include <vector>

class ConstantBufferResource : public Resource
{

public:

	static void SetForVertexShader (ID3D11DeviceContext & deviceContext, int startingSlot, std::vector<ConstantBufferResource*> buffers);

	static void SetForPixelShader (ID3D11DeviceContext & deviceContext, int startingSlot, std::vector<ConstantBufferResource*> buffers);

	ConstantBufferResource (int size);

	~ConstantBufferResource ();

	void Update (ID3D11DeviceContext & deviceContext, const void * pData, int cData) const;

	void SetForVertexShader (ID3D11DeviceContext & deviceContext, int slot) const;

	void SetForPixelShader (ID3D11DeviceContext & deviceContext, int slot) const;

	void SetForGeometryShader (ID3D11DeviceContext & deviceContext, int slot) const;

	void Create (ID3D11Device & device) override final;

	void Destroy () override final;

	bool IsCreated () const override final;

	int GetSize () const;

	static constexpr int GetBoundingBufferSize (int _size)
	{
		int remainder = _size % 16;
		if (remainder > 0)
		{
			return _size + 16 - remainder;
		}
		else
		{
			return _size;
		}
	}

private:

	ID3D11Buffer * m_pBuffer { nullptr };
	const UINT m_cBuffer;

};

template<typename T>
class ConstantBufferStructResource final : public ConstantBufferResource
{

public:

	T data;

	ConstantBufferStructResource (int _size) : ConstantBufferResource { _size }
	{
		GAME_ASSERT_MSG (_size <= sizeof (T), "Data size exceeds buffer size");
	}

	ConstantBufferStructResource () : ConstantBufferResource { GetBoundingBufferSize (sizeof (T)) }
	{}

	void Update (ID3D11DeviceContext & _deviceContext, int _cData) const
	{
		ConstantBufferResource::Update (_deviceContext, &data, _cData);
	}

	void Update (ID3D11DeviceContext & _deviceContext) const
	{
		ConstantBufferResource::Update (_deviceContext, &data, sizeof (T));
	}

};