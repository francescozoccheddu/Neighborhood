#include <Game/Game.hpp>
#include <Game/ResourceHandler.hpp>
#include <Game/Utils.hpp>
#include <Game/Exceptions.hpp>

//TODO Cleanup
#include <fstream>
#include <RapidJSON/document.h>


//#define SHADER_PATH(x) ("C:\\Users\\zocch\\Documents\\Visual Studio 2017\\Projects\\Neighborhood\\Debug\\" x)
#define SHADER_PATH(x) x

void Game::OnShow ()
{}

void Game::OnHide ()
{}

void Game::OnDeviceDestroyed ()
{
	ReleaseCOM (m_pVertexBuffer);
}

void loadMeshesJSON (float *& _pVerts, int & _cVerts, unsigned int *& _pInds, int & _cInds)
{
	//TODO Delete this
	std::ifstream file{ "meshes.json", std::ios::in };
	GAME_ASSERT (file.is_open ());
	std::string json ((std::istreambuf_iterator<char> (file)), std::istreambuf_iterator<char> ());
	rapidjson::Document document;
	document.Parse (json.c_str ());
	GAME_ASSERT (document.IsArray ());
	{
		rapidjson::Value& mesh{ document[0] };
		GAME_ASSERT (mesh.IsObject ());
		{
			rapidjson::Value& verts{ mesh["vertices"] };
			GAME_ASSERT (verts.IsArray ());
			float * buf = new float[verts.Size ()];
			for (rapidjson::SizeType i = 0; i < verts.Size (); i++)
			{
				buf[i] = verts[i].GetFloat ();
			}
			_pVerts = buf;
			_cVerts = verts.Size ();
		}
		{
			rapidjson::Value& inds{ mesh["indices"] };
			GAME_ASSERT (inds.IsArray ());
			unsigned int * buf = new unsigned int[inds.Size ()];
			for (rapidjson::SizeType i = 0; i < inds.Size (); i++)
			{
				buf[i] = inds[i].GetUint ();
			}
			_pInds = buf;
			_cInds = inds.Size ();
		}
	}
}

struct cbPerFrameBuffer
{
	DirectX::XMMATRIX mProjection;
	DirectX::XMMATRIX mView;
};

void Game::OnDeviceCreated ()
{
	ID3D11Device & device{ *GetDevice () };
	ID3D11DeviceContext & context{ *GetDeviceContext () };
	{
		// Shaders
		size_t vsLen, psLen;
		char * pVsData{ LoadBlob (SHADER_PATH (L"VertexShader.cso"), vsLen) };
		char * pPsData{ LoadBlob (SHADER_PATH (L"PixelShader.cso"), psLen) };
		GAME_COMC (device.CreateVertexShader (pVsData, vsLen, nullptr, &m_pVertexShader));
		GAME_COMC (device.CreatePixelShader (pPsData, psLen, nullptr, &m_pPixelShader));
		context.VSSetShader (m_pVertexShader, nullptr, 0);
		context.PSSetShader (m_pPixelShader, nullptr, 0);
		// Input layout
		D3D11_INPUT_ELEMENT_DESC desc;
		desc.SemanticName = "POSITION";
		desc.SemanticIndex = 0;
		desc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		desc.InputSlot = 0;
		desc.AlignedByteOffset = 0;
		desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		desc.InstanceDataStepRate = 0;
		GAME_COMC (device.CreateInputLayout (&desc, 1, pVsData, vsLen, &m_pInputLayout));
		context.IASetInputLayout (m_pInputLayout);
	}
	{
		// Buffer
		float * pVerts;
		unsigned int * pInds;
		int cVerts, cInds;
		UINT indSize{ sizeof (int) };
		loadMeshesJSON (pVerts, cVerts, pInds, cInds);
		{
			// Vertices
			ReleaseCOM (m_pVertexBuffer);
			UINT size{ sizeof (float) * 3 };
			D3D11_BUFFER_DESC desc;
			desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			desc.ByteWidth = cVerts / 3 * size;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;
			desc.StructureByteStride = size;
			desc.Usage = D3D11_USAGE_DEFAULT;
			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = pVerts;
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;
			GAME_COMC (device.CreateBuffer (&desc, &data, &m_pVertexBuffer));
			UINT stride{ size };
			UINT offset{ 0 };
			context.IASetVertexBuffers (0, 1, &m_pVertexBuffer, &stride, &offset);
		}
		{
			// Indices
			ReleaseCOM (m_pIndexBuffer);
			UINT size{ sizeof (unsigned int) * 1 };
			D3D11_BUFFER_DESC desc;
			desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
			desc.ByteWidth = cInds * size;
			desc.CPUAccessFlags = 0;
			desc.MiscFlags = 0;
			desc.StructureByteStride = 0;
			desc.Usage = D3D11_USAGE_DEFAULT;
			D3D11_SUBRESOURCE_DATA data;
			data.pSysMem = pInds;
			data.SysMemPitch = 0;
			data.SysMemSlicePitch = 0;
			GAME_COMC (device.CreateBuffer (&desc, &data, &m_pIndexBuffer));
			context.IASetIndexBuffer (m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
			m_cInds = cInds;
		}
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

void Game::OnRender (double _deltaTime)
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

	ID3D11DeviceContext& deviceContext = *GetDeviceContext ();

	deviceContext.UpdateSubresource (m_pConstantBuffer, 0, nullptr, &cb, 0, 0);

	deviceContext.VSSetConstantBuffers (0, 1, &m_pConstantBuffer);

	float color[]{ 0.1f,0.2f,0.3f,1.0f };
	deviceContext.ClearDepthStencilView (GetDepthStencilView (), D3D11_CLEAR_DEPTH, 1, 0);
	deviceContext.ClearRenderTargetView (GetRenderTargetView (), color);
	deviceContext.DrawIndexed (m_cInds, 0, 0);
}

void Game::OnSized (WindowSize size)
{
	m_CamView.position = DirectX::XMVectorSet (0.0f, 0.0f, -2.0f, 1.0f);
	m_CamProjection.aspectRatio = Camera::Projection::CalcAspectRatio (size.width, size.height);
	m_CamProjection.vFov = Camera::Projection::CalcVFov (1.2f, m_CamProjection.aspectRatio);
}

