#include <Game/Engine/GameListener.hpp>

#include <Game/Utils/Exceptions.hpp>
#include <Game/Utils/COMExceptions.hpp>
#include <Game/Utils/Storage.hpp>
#include <Game/Scene/Loader.hpp>
#include <Game/Scene/Mesh.hpp>

#define SHADER_PATH(x) x

GameListener::GameListener (ResourceHandler& _resourceHandler) : m_ResourceHandler { _resourceHandler } {}

void GameListener::OnDeviceDestroyed ()
{

}

struct cbPerFrameBuffer
{
	DirectX::XMMATRIX mProjection;
	DirectX::XMMATRIX mView;
};

void GameListener::OnDeviceCreated ()
{
	ID3D11Device & device { *m_ResourceHandler.GetDevice () };
	ID3D11DeviceContext & context { *m_ResourceHandler.GetDeviceContext () };
	{
		// Shaders
		int vsLen, psLen;
		char * pVsData { Storage::LoadBinaryFile (SHADER_PATH ("VertexShader.cso"), vsLen) };
		char * pPsData { Storage::LoadBinaryFile (SHADER_PATH ("PixelShader.cso"), psLen) };
		GAME_COMC (device.CreateVertexShader (pVsData, vsLen, nullptr, &m_pVertexShader));
		GAME_COMC (device.CreatePixelShader (pPsData, psLen, nullptr, &m_pPixelShader));
		context.VSSetShader (m_pVertexShader, nullptr, 0);
		context.PSSetShader (m_pPixelShader, nullptr, 0);
		// Input layout
		m_pInputLayout = Mesh::CreateInputLayout (device, pVsData, vsLen);
		context.IASetInputLayout (m_pInputLayout);
	}
	{
		// Buffer
		auto map { Scene::LoadFromJSON (Storage::LoadTextFile ("meshes.json")) };
		Mesh * mesh { map["Figure"] };
		m_pIndexBuffer = mesh->CreateD3DIndexBuffer (device);
		m_pVertexBuffer = mesh->CreateD3DVertexBuffer (device);
		m_cInds = mesh->GetIndicesCount ();
		Mesh::SetIAIndexBuffer (context, m_pIndexBuffer);
		Mesh::SetIAVertexBuffer (context, m_pVertexBuffer);
		delete mesh;
	}
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
		// State
		context.IASetPrimitiveTopology (D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
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
		m_CamView.position = {};
		m_CamView.turn = m_CamView.lookUp = 0.0f;
	}
	else
	{
		Gamepad::Thumb lt { m_Gamepad.GetThumb (Gamepad::Side::LEFT, {}) };
		Gamepad::Thumb rt { m_Gamepad.GetThumb (Gamepad::Side::RIGHT, {}) };
		float fdt = static_cast<float>(_deltaTime);
		float speed = m_Gamepad.AreButtonsPressed (XINPUT_GAMEPAD_A, false) ? 2.f : 1.f;
		m_CamView.Move (lt.x, lt.y, m_Gamepad.GetTrigger (Gamepad::Side::LEFT, 0.f) - m_Gamepad.GetTrigger (Gamepad::Side::RIGHT, 0.f), speed * fdt);
		m_CamView.turn += rt.x * fdt * 2.f;
		m_CamView.lookUp += rt.y * fdt * 2.f;
		m_CamView.ClampLookUp (DirectX::XMConvertToRadians (70.f));
	}

	m_CamProjection.Update ();

	cbPerFrameBuffer cb;
	cb.mProjection = m_CamProjection.Get ();
	cb.mView = m_CamView.Get ();

	ID3D11DeviceContext& deviceContext = *m_ResourceHandler.GetDeviceContext ();

	deviceContext.UpdateSubresource (m_pConstantBuffer, 0, nullptr, &cb, 0, 0);

	deviceContext.VSSetConstantBuffers (0, 1, &m_pConstantBuffer);

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
	deviceContext.DrawIndexed (m_cInds, 0, 0);
}

void GameListener::OnSized (WindowSize _size, DXGI_MODE_ROTATION _rotation)
{
	m_CamProjection.aspectRatio = Camera::Projection::CalcAspectRatio (_size.width, _size.height);
	m_CamProjection.vFov = Camera::Projection::CalcVFov (1.2f, m_CamProjection.aspectRatio);
}
