#pragma once

#include <Game/Scene/Scene.hpp>
#include <Game/Input/Gamepad.hpp>
#include <Game/Engine/EngineListener.hpp>
#include <Game/Resources/MeshResource.hpp>
#include <Game/Meshes/Meshes.hpp>

class Logic : public EngineListener
{

public:

	Logic ();

	void Update (double deltaTime);

	const Scene& GetScene () const;
private:


	MeshResource m_MeshSammy { Mesh_Sammy };
	Scene m_Scene;
	Gamepad m_Gamepad;

	virtual void OnDeviceDestroyed () override;

	virtual void OnDeviceCreated (const DeviceHolder& deviceHolder) override;

	virtual void OnSized (const DeviceHolder& deviceHolder) override;


};