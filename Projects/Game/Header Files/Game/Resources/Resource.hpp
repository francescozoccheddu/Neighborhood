#pragma once

#include <Game/Direct3D.hpp>

class Resource
{

public:

	virtual ~Resource () = default;

	bool IsCreated () const;

	void Create (ID3D11Device & device);

	void Destroy ();

protected:

	virtual void DoCreate (ID3D11Device & device) = 0;

	virtual void DoDestroy () = 0;

	void AssertCreated () const;

private:

	bool m_bCreated { false };

};