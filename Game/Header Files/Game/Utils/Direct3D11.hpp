#pragma once

#include <d3d11_4.h>

#pragma comment(lib, "d3d11.lib")

template<typename T>
void ReleaseCOM (T& comPtr)
{
	if (comPtr)
	{
		comPtr->Release ();
		comPtr = nullptr;
	}
}

