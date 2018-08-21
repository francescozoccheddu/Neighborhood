#pragma once

#include <Game/DirectXMath.hpp>

class MatrixWrapper
{

public:

	inline operator DirectX::XMMATRIX () const
	{
		return DirectX::XMLoadFloat4x4 (&m_Matrix);
	}

	inline operator const DirectX::XMFLOAT4X4 & () const
	{
		return m_Matrix;
	}

	inline DirectX::XMFLOAT4X4 operator*(const MatrixWrapper & _right) const
	{
		DirectX::XMFLOAT4X4 res;
		DirectX::XMStoreFloat4x4 (&res, DirectX::XMMatrixMultiply (*this, _right));
		return res;
	}

	virtual ~MatrixWrapper () = default;

protected:

	MatrixWrapper () = default;

	DirectX::XMFLOAT4X4 m_Matrix;

};

