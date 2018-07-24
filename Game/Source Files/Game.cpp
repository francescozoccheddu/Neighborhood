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
	ReleaseCOM (m_pBuffer);
}

void loadMeshesJSON (float *& _buf, int & _size)
{
	//TODO Delete this
	std::ifstream file{ "meshes.json", std::ios::in };
	GAME_ASSERT (file.is_open ());
	std::string json ((std::istreambuf_iterator<char> (file)), std::istreambuf_iterator<char> ());
	rapidjson::Document document;
	document.Parse (json.c_str ());
	GAME_ASSERT (document.IsArray ());
	rapidjson::Value& mesh{ document[0] };
	GAME_ASSERT (mesh.IsObject ());
	rapidjson::Value& verts{ mesh["vertices"] };
	GAME_ASSERT (verts.IsArray ());
	float * buf = new float[verts.Size ()];
	for (rapidjson::SizeType i = 0; i < verts.Size (); i++)
	{
		buf[i] = verts[i].GetFloat ();
	}
	_buf = buf;
	_size = verts.Size ();
}

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
		ReleaseCOM (m_pBuffer);
		float * verts;
		int count;
		UINT size{ sizeof (float) * 3 };
		loadMeshesJSON (verts, count);
		D3D11_BUFFER_DESC desc;
		desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		desc.ByteWidth = count * size;
		desc.CPUAccessFlags = 0;
		desc.MiscFlags = 0;
		desc.StructureByteStride = size;
		desc.Usage = D3D11_USAGE_DEFAULT;
		D3D11_SUBRESOURCE_DATA data;
		data.pSysMem = verts;
		data.SysMemPitch = 0;
		data.SysMemSlicePitch = 0;
		GAME_COMC (device.CreateBuffer (&desc, &data, &m_pBuffer));
		UINT stride{ size };
		UINT offset{ 0 };
		context.IASetVertexBuffers (0, 1, &m_pBuffer, &stride, &offset);
	}
	{
		// State
		context.IASetPrimitiveTopology (D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
}

void Game::OnRender (double deltaTime)
{
	float color[]{ 0.1f,0.2f,0.3f,1.0f };
	GetDeviceContext ()->ClearDepthStencilView (GetDepthStencilView (), D3D11_CLEAR_DEPTH, 1, 0);
	GetDeviceContext ()->ClearRenderTargetView (GetRenderTargetView (), color);
	GetDeviceContext ()->Draw (3, 0);
}

void Game::OnSized (WindowSize size)
{}

