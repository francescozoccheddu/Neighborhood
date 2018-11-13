#pragma once

#include <Game/Scene/Scene.hpp>
#include <Game/Input/Gamepad.hpp>
#include <Game/Engine/EngineListener.hpp>

class Logic : public EngineListener
{

public:

	Logic ();

	void Update (double deltaTime);

	const Scene& GetScene () const;
private:

	Scene m_Scene;
	Gamepad m_Gamepad;

	virtual void OnDeviceDestroyed () override;

	virtual void OnDeviceCreated (const DeviceHolder& deviceHolder) override;

	virtual void OnSized (const DeviceHolder& deviceHolder) override;


};