#pragma once

#include <Game/Direct3D.hpp>

class Resource
{

public:

	virtual ~Resource () = default;

	bool IsLoaded () const;

	void Load ();

	void Unload ();

	bool IsCreated () const;

	void Create (ID3D11Device & device);

	void Destroy ();

protected:

	virtual void DoLoad () = 0;

	virtual void DoUnload () = 0;

	virtual void DoCreate (ID3D11Device & device);

	virtual void DoDestroy () = 0;

	void AssertCreated () const;

private:

	bool m_bLoaded { false };
	bool m_bCreated { false };

};