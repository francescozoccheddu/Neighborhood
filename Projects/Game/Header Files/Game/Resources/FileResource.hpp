#pragma once

#include <Game/Resources/Resource.hpp>
#include <string>

class FileResource : public Resource
{

public:

	FileResource (const std::string& fileName);

	const std::string& GetFileName () const;

	bool IsLoaded () const;

	void Load ();

	void Unload ();

protected:

	virtual void DoCreateFromFile (ID3D11Device & device) = 0;

	virtual void DoLoad () = 0;

	virtual void DoUnload () = 0;

private:

	void DoCreate (ID3D11Device & device) override final;

	const std::string m_FileName;

	bool m_bLoaded { false };

};