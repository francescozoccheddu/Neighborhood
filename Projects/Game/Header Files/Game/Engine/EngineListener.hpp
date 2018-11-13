#pragma once

#include <Game/Direct3D.hpp>
#include <Game/Utils/WindowRect.hpp>

class DeviceHolder;

class EngineListener
{

public:

	virtual ~EngineListener () = default;

protected:

	friend class DeviceHolder;

	virtual void OnDeviceDestroyed () = 0;

	virtual void OnDeviceCreated (const DeviceHolder& deviceHolder) = 0;

	virtual void OnSized (const DeviceHolder& deviceHolder) = 0;

};