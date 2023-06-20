#include "jkRenderer.h"
#include "jkResources.h"
#include "jkTexture.h"
#include "jkMaterial.h"

namespace renderer
{	
	using namespace jk;
	using namespace jk::graphics;
	Vertex vertexes[4] = {};
	jk::graphics::ConstantBuffer* constantBuffer[(UINT)eCBType::End] = {};
	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState[(UINT)eSamplerType::End] = {};


	void SetupState()
	{
		// Input layout 정점 구조 정보를 넘겨줘야한다.
		D3D11_INPUT_ELEMENT_DESC arrLayout[3] = {};

		arrLayout[0].AlignedByteOffset = 0;
		arrLayout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		arrLayout[0].InputSlot = 0;
		arrLayout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		arrLayout[0].SemanticName = "POSITION";
		arrLayout[0].SemanticIndex = 0;

		arrLayout[1].AlignedByteOffset = 12;
		arrLayout[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		arrLayout[1].InputSlot = 0;
		arrLayout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		arrLayout[1].SemanticName = "COLOR";
		arrLayout[1].SemanticIndex = 0;

		arrLayout[2].AlignedByteOffset = 28;
		arrLayout[2].Format = DXGI_FORMAT_R32G32_FLOAT;
		arrLayout[2].InputSlot = 0;
		arrLayout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		arrLayout[2].SemanticName = "TEXCOORD";
		arrLayout[2].SemanticIndex = 0;


		Shader* shader = jk::Resources::Find<Shader>(L"TriangleShader");
		jk::graphics::GetDevice()->CreateInputLayout(arrLayout, 3
			, shader->GetVSCode()
			, shader->GetInputLayoutAddressOf());

		shader = jk::Resources::Find<Shader>(L"SpriteShader");
		jk::graphics::GetDevice()->CreateInputLayout(arrLayout, 3
			, shader->GetVSCode()
			, shader->GetInputLayoutAddressOf());


		//Sampler State
		D3D11_SAMPLER_DESC desc = {};
		desc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		desc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		desc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		GetDevice()->CreateSampler(&desc, samplerState[(UINT)eSamplerType::Point].GetAddressOf());
		GetDevice()->BindSampler(eShaderStage::PS, 0, samplerState[(UINT)eSamplerType::Point].GetAddressOf());

		desc.Filter = D3D11_FILTER_ANISOTROPIC;
		GetDevice()->CreateSampler(&desc, samplerState[(UINT)eSamplerType::Anisotropic].GetAddressOf());
		GetDevice()->BindSampler(eShaderStage::PS, 1, samplerState[(UINT)eSamplerType::Anisotropic].GetAddressOf());
	}

	void LoadBuffer()
	{
		// Vertex Buffer
		Mesh* mesh = new jk::Mesh();
		Resources::Insert(L"RectMesh", mesh);

		mesh->CreateVertexBuffer(vertexes, 4);

		std::vector<UINT> indexes = {};
		indexes.push_back(0);
		indexes.push_back(1);
		indexes.push_back(2);

		indexes.push_back(0);
		indexes.push_back(2);
		indexes.push_back(3);
		mesh->CreateIndexBuffer(indexes.data(), indexes.size());


		// Constant Buffer
		constantBuffer[(UINT)eCBType::Transform] = new ConstantBuffer(eCBType::Transform);
		constantBuffer[(UINT)eCBType::Transform]->Create(sizeof(Vector4));
	}

	void LoadShader()
	{
		Shader* shader = new jk::Shader();
		shader->Create(eShaderStage::VS, L"TriangleVS.hlsl", "main");
		shader->Create(eShaderStage::PS, L"TrianglePS.hlsl", "main");
		jk::Resources::Insert(L"TriangleShader", shader);


		Shader* spriteShader = new jk::Shader();
		spriteShader->Create(eShaderStage::VS, L"SpriteVS.hlsl", "main");
		spriteShader->Create(eShaderStage::PS, L"SpritePS.hlsl", "main");
		jk::Resources::Insert(L"SpriteShader", spriteShader);

		Texture* texture
			= Resources::Load<Texture>(L"Link", L"..\\Resources\\Texture\\Link.png");

		Material* spriteMateiral = new jk::graphics::Material();
		spriteMateiral->SetShader(spriteShader);
		spriteMateiral->SetTexture(texture);
		Resources::Insert(L"SpriteMaterial", spriteMateiral);
	}


	void Initialize()
	{
		vertexes[0].pos = Vector3(-0.5f, 0.5f, 0.0f);
		vertexes[0].color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
		vertexes[0].uv = Vector2(0.0f, 0.0f);

		vertexes[1].pos = Vector3(0.5f, 0.5f, 0.0f);
		vertexes[1].color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
		vertexes[1].uv = Vector2(1.0f, 0.0f);

		vertexes[2].pos = Vector3(0.5f, -0.5f, 0.0f);
		vertexes[2].color = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
		vertexes[2].uv = Vector2(1.0f, 1.0f);

		vertexes[3].pos = Vector3(-0.5f, -0.5f, 0.0f);
		vertexes[3].color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
		vertexes[3].uv = Vector2(0.0f, 1.0f);

		LoadBuffer();
		LoadShader();
		SetupState();

		Texture* texture
			= Resources::Load<Texture>(L"Smile", L"..\\Resources\\Texture\\Smile.png");
		texture
			= Resources::Load<Texture>(L"Link", L"..\\Resources\\Texture\\Link.png");

		texture->BindShader(eShaderStage::PS, 0);

		//vertexes[0].pos = Vector3(0.6f, 0.1f, 0.0f);
		//vertexes[0].color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
		//vertexes[1].pos = Vector3(0.7f, -0.1f, 0.0f);
		//vertexes[1].color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
		//vertexes[2].pos = Vector3(0.5f, -0.1f, 0.0f);
		//vertexes[2].color = Vector4(0.0f, 0.0f, 1.0f, 1.0f);


		////원
		//float angle = 0;
		//float radius = 0.1f;
		//Vector3 Pos = Vector3(0.0f, 0.0f, 0.0f);

		//for (UINT i = 0; i < 360; i++)
		//{
		//	float X = Pos.x;
		//	float Y = Pos.y;
		//	

		//	if (i % 3 == 2)
		//	{
		//		vertexes[i].pos = Pos;
		//		continue;
		//	}

		//	//angle +=  Time::DeltaTime() * 10;
		//	float radian = (360 - i) * 3.14 / 180;
		//	X += radius * cos(radian);
		//	Y += radius * sin(radian);

		//	vertexes[i].pos = Vector3(X, Y, 0.0f);
		//	vertexes[i].color = Vector4(0.0f, 0.0f, 1.0f, 0.0f);
		//}


		////마름모
		//vertexes[360].pos = Vector3(0.6f, 0.1f, 0.0f);
		//vertexes[360].color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
		//vertexes[361].pos = Vector3(0.7f, -0.1f, 0.0f);
		//vertexes[361].color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
		//vertexes[362].pos = Vector3(0.5f, -0.1f, 0.0f);
		//vertexes[362].color = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
		//vertexes[363].pos = Vector3(0.7f, -0.1f, 0.0f);
		//vertexes[363].color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
		//vertexes[364].pos = Vector3(0.6f, -0.3f, 0.0f);
		//vertexes[364].color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
		//vertexes[365].pos = Vector3(0.5f, -0.1f, 0.0f);
		//vertexes[365].color = Vector4(0.0f, 0.0f, 1.0f, 1.0f);


		////사각형
		//vertexes[366].pos = Vector3(0.3f, 0.3f, 0.0f);
		//vertexes[366].color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
		//vertexes[367].pos = Vector3(0.4f, 0.3f, 0.0f);
		//vertexes[367].color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
		//vertexes[368].pos = Vector3(0.4f, 0.2f, 0.0f);
		//vertexes[368].color = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
		//vertexes[369].pos = Vector3(0.4f, 0.2f, 0.0f);
		//vertexes[369].color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
		//vertexes[370].pos = Vector3(0.3f, 0.2f, 0.0f);
		//vertexes[370].color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
		//vertexes[371].pos = Vector3(0.3f, 0.3f, 0.0f);
		//vertexes[371].color = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
	}
	void Release()
	{
		for (ConstantBuffer* buff : constantBuffer)
		{
			if (buff == nullptr)
				continue;

			delete buff;
			buff = nullptr;
		}
	}
}


