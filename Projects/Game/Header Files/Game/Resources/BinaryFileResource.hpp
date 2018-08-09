#pragma once

#include <Game/Resources/FileResource.hpp>

class BinaryFileResource : public FileResource
{

protected:

	using FileResource::FileResource;

	void DoLoad () override final;

	void DoUnload () override final;

	void DoCreate (ID3D11Device & device) override final;

	virtual void DoCreateFromBinary (ID3D11Device & device, const void * pData, int cData) = 0;

private:

	void * m_pData;
	int m_cData;

};