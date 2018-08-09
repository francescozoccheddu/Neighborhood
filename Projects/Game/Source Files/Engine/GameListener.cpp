#include <Game/Engine/GameListener.hpp>

#include <Game/Utils/Exceptions.hpp>
#include <Game/Utils/COMExceptions.hpp>

#define RESOURCES_PATH "Resources/"
#define SHADER_PATH(x) RESOURCES_PATH "Shaders/" x
#define MESH_PATH RESOURCES_PATH "meshes.json"
#define TEXTURE_PATH(x) RESOURCES_PATH "Textures/" x

GameListener::GameListener (ResourceHandler& _resourceHandler) : m_ResourceHandler { _resourceHandler }
{
	m_CamView.position = { 0.0, 0.0f, -2.0f };
	m_ResourceHolder.AddMesh ("Sammy");
	m_ResourceHolder.AddTexture ("Sammy");
	m_ResourceHolder.AddShader ("default_vertex", ShaderResource::ShaderType::VERTEX);
	m_ResourceHolder.AddShader ("default_pixel", ShaderResource::ShaderType::PIXEL);
	m_ResourceHolder.Load ();
}

void GameListener::OnDeviceDestroyed ()
{
	m_ResourceHolder.Destroy ();
	m_pSampler->Release ();
	m_pConstantBuffer->Release ();
}

struct cbPerFrameBuffer
{
	DirectX::XMMATRIX mProjection;
	DirectX::XMMATRIX mView;
};

void GameListener::OnDeviceCreated ()
{
	ID3D11Device & device { *m_ResourceHandler.GetDevice () };
	m_ResourceHolder.Create (device);
	{
		// Constant buffers
		D3D11_BUFFER_DESC desc;
		desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		desc.ByteWidth = sizeof (cbPerFrameBuffer);
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = 0;
		desc.Usage = D3D11_USAGE_DEFAULT;
		GAME_COMC (device.CreateBuffer (&desc, nullptr, &m_pConstantBuffer));
	}
	{
		D3D11_SAMPLER_DESC desc {};
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		desc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		desc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		desc.MinLOD = 0;
		desc.MaxLOD = D3D11_FLOAT32_MAX;
		GAME_COMC (device.CreateSamplerState (&desc, &m_pSampler));
	}
}

void GameListener::OnRender (double _deltaTime)
{
	m_Gamepad.Update ();
	if (!m_Gamepad.IsConnected ())
	{
		DWORD iUser;
		if (Gamepad::FindNextController (0, iUser))
		{
			m_Gamepad.SetUserIndex (iUser);
			m_Gamepad.Update ();
		}
	}

	if (m_Gamepad.AreButtonsPressed (XINPUT_GAMEPAD_B, false))
	{
		m_CamView.position = { 0.0f, 0.0f, -2.0f };
		m_CamView.turn = m_CamView.lookUp = 0.0f;
	}
	else
	{
		Gamepad::Thumb lt { m_Gamepad.GetThumb (Gamepad::Side::LEFT, {}) };
		Gamepad::Thumb rt { m_Gamepad.GetThumb (Gamepad::Side::RIGHT, {}) };
		float fdt = static_cast<float>(_deltaTime);
		float speed = m_Gamepad.AreButtonsPressed (XINPUT_GAMEPAD_A, false) ? 2.f : 1.f;
		float angSpeed = DirectX::XMConvertToRadians (90);
		m_CamView.Move (lt.x, lt.y, m_Gamepad.GetTrigger (Gamepad::Side::RIGHT, 0.f) - m_Gamepad.GetTrigger (Gamepad::Side::LEFT, 0.f), speed * fdt);
		m_CamView.turn += rt.x * fdt * angSpeed;
		m_CamView.lookUp += -rt.y * fdt * angSpeed;
		m_CamView.ClampLookUp (DirectX::XMConvertToRadians (70.f));
	}

	m_CamView.Update ();
	m_CamProjection.Update ();

	const MeshResource& mesh { m_ResourceHolder.GetMesh ("Sammy") };

	cbPerFrameBuffer cb;
	cb.mProjection = m_CamProjection.Get ();
	cb.mView = m_CamView.Get ();

	ID3D11DeviceContext& deviceContext = *m_ResourceHandler.GetDeviceContext ();

	mesh.SetBuffers (deviceContext);
	m_ResourceHolder.GetShader ("default_vertex").SetShaderAndInputLayout (deviceContext);
	m_ResourceHolder.GetShader ("default_pixel").SetShaderAndInputLayout (deviceContext);
	m_ResourceHolder.GetTexture ("Sammy").SetShaderResource (deviceContext, 0);

	deviceContext.UpdateSubresource (m_pConstantBuffer, 0, nullptr, &cb, 0, 0);
	deviceContext.IASetPrimitiveTopology (D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	deviceContext.VSSetConstantBuffers (0, 1, &m_pConstantBuffer);
	deviceContext.PSSetSamplers (0, 1, &m_pSampler);

	static double time = 0.0;
	time += _deltaTime;
	if (time > DirectX::XM_2PI)
	{
		time -= DirectX::XM_2PI;
	}
	float hue = static_cast<float>(std::cos (time) / -2.0 + 0.5);

	float color[] { hue / 2.0f, 0.2f, 0.3f, 1.0f };
	deviceContext.ClearDepthStencilView (m_ResourceHandler.GetDepthStencilView (), D3D11_CLEAR_DEPTH, 1, 0);
	deviceContext.ClearRenderTargetView (m_ResourceHandler.GetRenderTargetView (), color);
	deviceContext.DrawIndexed (mesh.GetIndicesCount (), 0, 0);
}

void GameListener::OnSized (WindowSize _size, DXGI_MODE_ROTATION _rotation)
{
	m_CamProjection.aspectRatio = Camera::Projection::CalcAspectRatio (_size.width, _size.height);
	m_CamProjection.vFov = Camera::Projection::CalcVFov (1.2f, m_CamProjection.aspectRatio);
}

