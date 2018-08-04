#include <Game/Engine/GameListener.hpp>
#include <Game/Engine/ResourceHandler.hpp>
#include <Game/Utils/Exceptions.hpp>
#include <Game/Utils/COMExceptions.hpp>
#include <Game/Utils/Storage.hpp>
#include <Game/Scene/Mesh.hpp>
#include <Game/Scene/Loader.hpp>

//TODO Cleanup
#include <fstream>


//#define SHADER_PATH(x) ("C:\\Users\\zocch\\Documents\\Visual Studio 2017\\Projects\\Neighborhood\\Debug\\" x)
#define SHADER_PATH(x) x

GameListener::GameListener (ResourceHandler& _resourceHandler) : m_ResourceHandler { _resourceHandler } {}

void GameListener::OnDeviceDestroyed ()
{
	ReleaseCOM (m_pVertexBuffer);
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
		D3D11_INPUT_ELEMENT_DESC desc[2];
		desc[0].SemanticName = "POSITION";
		desc[0].SemanticIndex = 0;
		desc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		desc[0].InputSlot = 0;
		desc[0].AlignedByteOffset = 0;
		desc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		desc[0].InstanceDataStepRate = 0;
		desc[1].SemanticName = "NORMAL";
		desc[1].SemanticIndex = 0;
		desc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		desc[1].InputSlot = 0;
		desc[1].AlignedByteOffset = sizeof (DirectX::XMFLOAT3);
		desc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		desc[1].InstanceDataStepRate = 0;
		GAME_COMC (device.CreateInputLayout (desc, ARRAYSIZE (desc), pVsData, vsLen, &m_pInputLayout));
		context.IASetInputLayout (m_pInputLayout);
	}
	{
		// Buffer
		auto map { Scene::LoadFromJSON (Storage::LoadTextFile ("meshes.json")) };
		Mesh mesh { map["Figure"] };
		m_pIndexBuffer = mesh.CreateD3DIndexBuffer (device);
		m_pVertexBuffer = mesh.CreateD3DVertexBuffer (device);
		m_cInds = mesh.GetIndicesCount ();
		Mesh::SetIAIndexBuffer (context, m_pIndexBuffer);
		Mesh::SetIAVertexBuffer (context, m_pVertexBuffer);
	}
	{
		// Constant buffers
		ReleaseCOM (m_pConstantBuffer);
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

void GameListener::OnRender (float _deltaTime)
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

	m_CamView.UpdateFlyingView (_deltaTime, m_Gamepad);
	m_CamProjection.Update ();

	cbPerFrameBuffer cb;
	cb.mProjection = m_CamProjection.Get ();
	cb.mView = m_CamView.Get ();

	ID3D11DeviceContext& deviceContext = *m_ResourceHandler.GetDeviceContext ();

	deviceContext.UpdateSubresource (m_pConstantBuffer, 0, nullptr, &cb, 0, 0);

	deviceContext.VSSetConstantBuffers (0, 1, &m_pConstantBuffer);

	float color[] { 0.1f, 0.2f, 0.3f, 1.0f };
	deviceContext.ClearDepthStencilView (m_ResourceHandler.GetDepthStencilView (), D3D11_CLEAR_DEPTH, 1, 0);
	deviceContext.ClearRenderTargetView (m_ResourceHandler.GetRenderTargetView (), color);
	deviceContext.DrawIndexed (m_cInds, 0, 0);
}

void GameListener::OnSized (WindowSize size)
{
	m_CamView.position = DirectX::XMVectorSet (0.0f, 0.0f, -2.0f, 1.0f);
	m_CamProjection.aspectRatio = Camera::Projection::CalcAspectRatio (size.width, size.height);
	m_CamProjection.vFov = Camera::Projection::CalcVFov (1.2f, m_CamProjection.aspectRatio);
}

