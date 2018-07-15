#pragma once

#include <string>
#include "Direct3D11.h"

struct WindowSize
{
	int width;
	int height;
};

class WindowListener
{
public:

	virtual void OnTick (double deltaTime) = 0;

	virtual void OnShow () = 0;

	virtual void OnHide () = 0;

	virtual void OnSize (WindowSize size) = 0;

	virtual void OnCreate () = 0;

	virtual void OnDestroy () = 0;

};


