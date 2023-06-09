#include "jkRenderer.h"
#include "jkResources.h"
#include "jkTexture.h"
#include "jkMaterial.h"

namespace jk::renderer
{	
	using namespace jk;
	using namespace jk::graphics;
	Vertex vertexes[4] = {};
	jk::graphics::ConstantBuffer* constantBuffer[(UINT)eCBType::End] = {};

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState[(UINT)eSamplerType::End] = {};
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStates[(UINT)eRSType::End] = {};
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilStates[(UINT)eDSType::End] = {};
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendStates[(UINT)eBSType::End] = {};

	std::vector<jk::Camera*> cameras = {};
	std::vector<DebugMesh> debugMeshs = {};

	void SetupState()
	{
		// Input layout 정점 구조 정보를 넘겨줘야한다.
#pragma region InputLayout		
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

		std::shared_ptr<Shader> shader = jk::Resources::Find<Shader>(L"TriangleShader");
		jk::graphics::GetDevice()->CreateInputLayout(arrLayout, 3
			, shader->GetVSCode()
			, shader->GetInputLayoutAddressOf());


		shader = jk::Resources::Find<Shader>(L"SpriteShader");
		jk::graphics::GetDevice()->CreateInputLayout(arrLayout, 3
			, shader->GetVSCode()
			, shader->GetInputLayoutAddressOf());


		shader = jk::Resources::Find<Shader>(L"Grid_Shader");
		jk::graphics::GetDevice()->CreateInputLayout(arrLayout, 3
			, shader->GetVSCode()
			, shader->GetInputLayoutAddressOf());


		shader = jk::Resources::Find<Shader>(L"DebugShader");
		jk::graphics::GetDevice()->CreateInputLayout(arrLayout, 3
			, shader->GetVSCode()
			, shader->GetInputLayoutAddressOf());

		
		shader = jk::Resources::Find<Shader>(L"Tile_Shader");
		jk::graphics::GetDevice()->CreateInputLayout(arrLayout, 3
			, shader->GetVSCode()
			, shader->GetInputLayoutAddressOf());


		shader = jk::Resources::Find<Shader>(L"Move_Shader");
		jk::graphics::GetDevice()->CreateInputLayout(arrLayout, 3
			, shader->GetVSCode()
			, shader->GetInputLayoutAddressOf());

#pragma endregion

		//Sampler State
#pragma region Sampler State		
		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		GetDevice()->CreateSamplerState(&samplerDesc, samplerState[(UINT)eSamplerType::Point].GetAddressOf());
		GetDevice()->BindSampler(eShaderStage::PS, 0, samplerState[(UINT)eSamplerType::Point].GetAddressOf());

		samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
		GetDevice()->CreateSamplerState(&samplerDesc, samplerState[(UINT)eSamplerType::Anisotropic].GetAddressOf());
		GetDevice()->BindSampler(eShaderStage::PS, 1, samplerState[(UINT)eSamplerType::Anisotropic].GetAddressOf());
#pragma endregion

		//Rasterizer State
#pragma region Rasterizer State
		D3D11_RASTERIZER_DESC rasterizerDesc = {};
		rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
		GetDevice()->CreateRasterizeState(&rasterizerDesc
			, rasterizerStates[(UINT)eRSType::SolidBack].GetAddressOf());

		rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;
		GetDevice()->CreateRasterizeState(&rasterizerDesc
			, rasterizerStates[(UINT)eRSType::SolidFront].GetAddressOf());

		rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
		GetDevice()->CreateRasterizeState(&rasterizerDesc
			, rasterizerStates[(UINT)eRSType::SolidNone].GetAddressOf());

		rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
		rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
		GetDevice()->CreateRasterizeState(&rasterizerDesc
			, rasterizerStates[(UINT)eRSType::WireframeNone].GetAddressOf());
#pragma endregion

		//Depth Stencil State
#pragma region Depth Stencil State
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};

		//less
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.StencilEnable = false;

		GetDevice()->CreateDepthStencilState(&depthStencilDesc
			, depthStencilStates[(UINT)eDSType::Less].GetAddressOf());

		//Greater
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_GREATER;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.StencilEnable = false;

		GetDevice()->CreateDepthStencilState(&depthStencilDesc
			, depthStencilStates[(UINT)eDSType::Greater].GetAddressOf());

		//No Write
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.StencilEnable = false;

		GetDevice()->CreateDepthStencilState(&depthStencilDesc
			, depthStencilStates[(UINT)eDSType::NoWrite].GetAddressOf());

		//None
		depthStencilDesc.DepthEnable = false;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilDesc.StencilEnable = false;

		GetDevice()->CreateDepthStencilState(&depthStencilDesc
			, depthStencilStates[(UINT)eDSType::None].GetAddressOf());
#pragma endregion

		//Blend State
#pragma region Blend State
		D3D11_BLEND_DESC blendDesc = {};

		//default
		blendStates[(UINT)eBSType::Default] = nullptr;

		// Alpha Blend
		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;
		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		GetDevice()->CreateBlendState(&blendDesc
			, blendStates[(UINT)eBSType::AlphaBlend].GetAddressOf());

		// one one
		blendDesc.AlphaToCoverageEnable = false;
		blendDesc.IndependentBlendEnable = false;

		blendDesc.RenderTarget[0].BlendEnable = true;
		blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		GetDevice()->CreateBlendState(&blendDesc
			, blendStates[(UINT)eBSType::OneOne].GetAddressOf());

#pragma endregion
	}

	void LoadMesh()
	{
		std::vector<Vertex> vertexes = {};
		std::vector<UINT> indexes = {};
	#pragma region RECT

		//RECT
		vertexes.resize(4);
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

		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();
		Resources::Insert(L"RectMesh", mesh);
		mesh->CreateVertexBuffer(vertexes.data(), (UINT)vertexes.size());

	
		indexes.push_back(0);
		indexes.push_back(1);
		indexes.push_back(2);

		indexes.push_back(0);
		indexes.push_back(2);
		indexes.push_back(3);
		mesh->CreateIndexBuffer(indexes.data(), (UINT)indexes.size());		

		// Rect Debug Mesh
		std::shared_ptr<Mesh> rectDebug = std::make_shared<Mesh>();
		Resources::Insert(L"DebugRect", rectDebug);
		rectDebug->CreateVertexBuffer(vertexes.data(), (UINT)vertexes.size());
		rectDebug->CreateIndexBuffer(indexes.data(), (UINT)indexes.size());
	#pragma endregion

	#pragma region Circle
		// Circle Debug Mesh
		vertexes.clear();
		indexes.clear();

		Vertex center = {};
		center.pos = Vector3(0.0f, 0.0f, 0.0f);
		center.color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
		vertexes.push_back(center);

		int iSlice = 40;
		float fRadius = 0.5f;
		float fTheta = XM_2PI / (float)iSlice;

		for (int i = 0; i < iSlice; ++i)
		{
			center.pos = Vector3(fRadius * cosf(fTheta * (float)i)
				, fRadius * sinf(fTheta * (float)i)
				, 0.0f);
			center.color = Vector4(0.0f, 1.0f, 0.0f, 1.f);
			vertexes.push_back(center);
		}

		//for (UINT i = 0; i < (UINT)iSlice; ++i)
		//{
		//	indexes.push_back(0);
		//	if (i == iSlice - 1)
		//	{
		//		indexes.push_back(1);
		//	}
		//	else
		//	{
		//		indexes.push_back(i + 2);
		//	}
		//	indexes.push_back(i + 1);
		//}

		for (int i = 0; i < vertexes.size() - 2; ++i)
		{
			indexes.push_back(i + 1);
		}
		indexes.push_back(1);

		std::shared_ptr<Mesh> circleDebug = std::make_shared<Mesh>();
		Resources::Insert(L"DebugCircle", circleDebug);
		circleDebug->CreateVertexBuffer(vertexes.data(), (UINT)vertexes.size());
		circleDebug->CreateIndexBuffer(indexes.data(), (UINT)indexes.size());
	#pragma endregion

	}

	void LoadBuffer()
	{
		// Transform ConBuffer
		constantBuffer[(UINT)eCBType::Transform] = new ConstantBuffer(eCBType::Transform);
		constantBuffer[(UINT)eCBType::Transform]->Create(sizeof(TransformCB));

		// Material ConBuffer
		constantBuffer[(UINT)eCBType::Material] = new ConstantBuffer(eCBType::Material);
		constantBuffer[(UINT)eCBType::Material]->Create(sizeof(MaterialCB));

		// Grid ConBuffer
		constantBuffer[(UINT)eCBType::Grid] = new ConstantBuffer(eCBType::Grid);
		constantBuffer[(UINT)eCBType::Grid]->Create(sizeof(TransformCB));

		// Move ConBuffer
		constantBuffer[(UINT)eCBType::Move] = new ConstantBuffer(eCBType::Move);
		constantBuffer[(UINT)eCBType::Move]->Create(sizeof(MoveCB));

		// UV ConBuffer
		constantBuffer[(UINT)eCBType::UV] = new ConstantBuffer(eCBType::UV);
		constantBuffer[(UINT)eCBType::UV]->Create(sizeof(TileMap_CB));
	}

	void LoadShader()
	{
		std::shared_ptr<Shader> shader = std::make_shared<Shader>();
		shader->Create(eShaderStage::VS, L"TriangleVS.hlsl", "main");
		shader->Create(eShaderStage::PS, L"TrianglePS.hlsl", "main");
		jk::Resources::Insert(L"TriangleShader", shader);

		std::shared_ptr<Shader> spriteShader = std::make_shared<Shader>();
		spriteShader->Create(eShaderStage::VS, L"SpriteVS.hlsl", "main");
		spriteShader->Create(eShaderStage::PS, L"SpritePS.hlsl", "main");
		jk::Resources::Insert(L"SpriteShader", spriteShader);


		std::shared_ptr<Shader> girdShader = std::make_shared<Shader>();
		girdShader->Create(eShaderStage::VS, L"GridVS.hlsl", "main");
		girdShader->Create(eShaderStage::PS, L"GridPS.hlsl", "main");
		jk::Resources::Insert(L"Grid_Shader", girdShader);

		std::shared_ptr<Shader> debugShader = std::make_shared<Shader>();
		debugShader->Create(eShaderStage::VS, L"DebugVS.hlsl", "main");
		debugShader->Create(eShaderStage::PS, L"DebugPS.hlsl", "main");
		debugShader->SetTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_LINESTRIP);
		debugShader->SetRSState(eRSType::SolidNone);
		//debugShader->SetDSState(eDSType::NoWrite);
		jk::Resources::Insert(L"DebugShader", debugShader);

		//구름
		std::shared_ptr<Shader> moveShader = std::make_shared<Shader>();
		moveShader->Create(eShaderStage::VS, L"MoveVS.hlsl", "main");
		moveShader->Create(eShaderStage::PS, L"MovePS.hlsl", "main");
		jk::Resources::Insert(L"Move_Shader", moveShader);


		//타일
#pragma region Tile_map_cmarkup
		std::shared_ptr<Shader> tile_shader = std::make_shared<Shader>();
		tile_shader->Create(eShaderStage::VS, L"TileMapVS.hlsl", "main");
		tile_shader->Create(eShaderStage::PS, L"TileMapPS.hlsl", "main");
		jk::Resources::Insert(L"Tile_Shader", tile_shader);
#pragma endregion	
	}

	void LoadMaterial()
	{
		std::shared_ptr<Shader> spriteShader
			= Resources::Find<Shader>(L"SpriteShader");
		std::shared_ptr<Shader> gridShader
			= Resources::Find<Shader>(L"Grid_Shader");
		std::shared_ptr<Shader> moveShader
			= Resources::Find<Shader>(L"Move_Shader");
		std::shared_ptr<Shader> tile_shader
			= Resources::Find<Shader>(L"Tile_Shader");

	#pragma region Public
		#pragma region Mouse
			std::shared_ptr<Texture> texture
				= Resources::Load<Texture>(L"mouse", L"..\\Resources\\Texture\\Mouse_Cursor.png");
			std::shared_ptr<Material> material = std::make_shared<Material>();
			material->SetShader(spriteShader);
			material->SetTexture(texture);
			Resources::Insert(L"Mouse", material);
		#pragma endregion

		#pragma region Grid	
			material = std::make_shared<Material>();
			material->SetShader(gridShader);
			Resources::Insert(L"GridMaterial", material);
		#pragma endregion

		#pragma region	UI	
				#pragma region UI_PlayerState
						texture = Resources::Load<Texture>(L"state_ui", L"..\\Resources\\Texture\\UI\\State_UI\\State_UI.png");
						material = std::make_shared<Material>(); material->SetShader(spriteShader);	material->SetTexture(texture);
						material->SetRenderingMode(eRenderingMode::Transparent); Resources::Insert(L"State_UI", material);
				#pragma endregion

				#pragma region UI_PlaerFace
						texture = Resources::Load<Texture>(L"skul_ui", L"..\\Resources\\Texture\\UI\\Face_UI\\Skul_UI.png");
						material = std::make_shared<Material>();material->SetShader(spriteShader);	material->SetTexture(texture);
						material->SetRenderingMode(eRenderingMode::Transparent);Resources::Insert(L"Skul_UI", material);
				#pragma endregion

				#pragma region UI_Player_HP_Clean
						texture = Resources::Load<Texture>(L"healthbar_ui", L"..\\Resources\\Texture\\UI\\Hp_Bar\\Player_HealthBar.png");
						material = std::make_shared<Material>(); material->SetShader(spriteShader);	material->SetTexture(texture);
						material->SetRenderingMode(eRenderingMode::Transparent); Resources::Insert(L"HealthUar_Ui", material);
				#pragma endregion

				#pragma region UI_Player_HP_Damege
						texture = Resources::Load<Texture>(L"damagebar_ui", L"..\\Resources\\Texture\\UI\\Hp_Bar\\Player_HealthBar_Damage.png");
						material = std::make_shared<Material>(); material->SetShader(spriteShader);	material->SetTexture(texture);
						material->SetRenderingMode(eRenderingMode::Transparent); Resources::Insert(L"DamageBar_Ui", material);
				#pragma endregion
		#pragma endregion

		#pragma region Player
			#pragma region Skul_Base
				texture = Resources::Load<Texture>(L"basic_skul", L"..\\Resources\\Texture\\Player\\Normal\\Idle\\Idle_0.png");
				material = std::make_shared<Material>(); material->SetShader(spriteShader);	material->SetTexture(texture);
				material->SetRenderingMode(eRenderingMode::Transparent); Resources::Insert(L"Basic_Skul", material);
			#pragma endregion
		#pragma endregion

	#pragma endregion

	#pragma region Title
			texture	= Resources::Load<Texture>(L"title_image", L"..\\Resources\\Texture\\Title\\Title_Art2.png");
			material = std::make_shared<Material>(); material->SetShader(spriteShader);	material->SetTexture(texture);	
			Resources::Insert(L"Title_Image", material);

			texture = Resources::Load<Texture>(L"title_logo", L"..\\Resources\\Texture\\Title\\Title_Logo.png");
			material = std::make_shared<Material>(); material->SetShader(spriteShader);	material->SetTexture(texture);	
			material->SetRenderingMode(eRenderingMode::Transparent);  Resources::Insert(L"Title_Logo", material);

			texture = Resources::Load<Texture>(L"title_mlogo2", L"..\\Resources\\Texture\\Title\\Title_Logo2.png");
			material = std::make_shared<Material>(); material->SetShader(spriteShader);	material->SetTexture(texture);	
			material->SetRenderingMode(eRenderingMode::Transparent); Resources::Insert(L"Title_Mini_Logo", material);
	#pragma endregion		

	#pragma region StartScene

		#pragma region In_Castle
			#pragma region Devil(back)
						texture	= Resources::Load<Texture>(L"DevilCastle", L"..\\Resources\\Texture\\Devil_Catle\\Catle_wall_Back.png");
						material = std::make_shared<Material>();
						material->SetShader(spriteShader);
						material->SetTexture(texture);
						material->SetRenderingMode(eRenderingMode::Transparent);
						Resources::Insert(L"Catle_wall_Back", material);
			#pragma endregion

			#pragma region Devil(mid)
						texture = Resources::Load<Texture>(L"Catle_wall_Front", L"..\\Resources\\Texture\\Devil_Catle\\Catle_wall_Front_01.png");
						material = std::make_shared<Material>();
						material->SetShader(spriteShader);
						material->SetTexture(texture);
						material->SetRenderingMode(eRenderingMode::Transparent);
						Resources::Insert(L"Catle_wall_Front_01", material);
			#pragma endregion

			#pragma region in(fore_ground)
						texture = Resources::Load<Texture>(L"in_front", L"..\\Resources\\Texture\\Devil_Catle\\In_Fore_GR.png");
						material = std::make_shared<Material>();	material->SetShader(spriteShader);	material->SetTexture(texture);
						material->SetRenderingMode(eRenderingMode::Transparent); Resources::Insert(L"In_Fore_GR", material);
			#pragma endregion	

			#pragma region Devil(Devil_Chair)
						texture	= Resources::Load<Texture>(L"Devil_chair", L"..\\Resources\\Texture\\Devil_Catle\\Devil_chair.png");
						material = std::make_shared<Material>();
						material->SetShader(spriteShader);
						material->SetTexture(texture);
						material->SetRenderingMode(eRenderingMode::Transparent);
						Resources::Insert(L"Devil_Chair", material);
			#pragma endregion
		#pragma endregion


		#pragma region out_Castle
			#pragma region Out(fore_ground)
						texture = Resources::Load<Texture>(L"out_front", L"..\\Resources\\Texture\\Devil_Catle\\Out_Fore_GR.png");
						material = std::make_shared<Material>(); material->SetShader(spriteShader); material->SetTexture(texture);
						material->SetRenderingMode(eRenderingMode::Transparent); Resources::Insert(L"Out_Fore_GR", material);
			#pragma endregion	

		#pragma endregion


			#pragma region Cloud				
			
						texture = Resources::Load<Texture>(L"Cloud_devil", L"..\\Resources\\Texture\\Effect\\Cloud_devil.png");
						material = std::make_shared<Material>();
						material->SetShader(moveShader);
						material->SetTexture(texture);
						material->SetRenderingMode(eRenderingMode::Transparent);
						Resources::Insert(L"Cloud_Devil", material);
			#pragma endregion



			#pragma region Tile_map(Mark_Up)
						texture = Resources::Load<Texture>(L"DG_Tiles", L"..\\Resources\\Tile\\DG_Tile.png");
						material = std::make_shared<Material>();
						material->SetShader(tile_shader);
						material->SetTexture(texture);
						material->SetRenderingMode(eRenderingMode::Transparent);
						Resources::Insert(L"DG_Tile", material);
			#pragma endregion



			#pragma region PlayScene_Devil(background_materials)
						//{
						//	std::shared_ptr<Texture> texture
						//		= Resources::Load<Texture>(L"Catle_wall_Front2", L"..\\Resources\\Texture\\Devil_Catle\\Catle_wall_Front_02.png");
						//	std::shared_ptr<Material> spriteMateiral = std::make_shared<Material>();
						//	spriteMateiral->SetShader(spriteShader);
						//	spriteMateiral->SetTexture(texture);
						//	Resources::Insert(L"Catle_wall_Front_02", spriteMateiral);
						//}

						//{
						//	std::shared_ptr<Texture> texture
						//		= Resources::Load<Texture>(L"Catle_wall_Front3", L"..\\Resources\\Texture\\Devil_Catle\\Catle_wall_Front_03.png");
						//	std::shared_ptr<Material> spriteMateiral = std::make_shared<Material>();
						//	spriteMateiral->SetShader(spriteShader);
						//	spriteMateiral->SetTexture(texture);
						//	Resources::Insert(L"Catle_wall_Front_03", spriteMateiral);
						//}

						//{
						//	std::shared_ptr<Texture> texture
						//		= Resources::Load<Texture>(L"Catle_wall_Front4", L"..\\Resources\\Texture\\Devil_Catle\\Catle_wall_Front_04.png");
						//	std::shared_ptr<Material> spriteMateiral = std::make_shared<Material>();
						//	spriteMateiral->SetShader(spriteShader);
						//	spriteMateiral->SetTexture(texture);
						//	Resources::Insert(L"Catle_wall_Front_04", spriteMateiral);
							//}
			#pragma endregion

	#pragma endregion

	#pragma region Stage1(Back)
			texture = Resources::Load<Texture>(L"king1", L"..\\Resources\\Texture\\Stage1\\King1.png");
			material = std::make_shared<Material>();
			material->SetShader(spriteShader);
			material->SetTexture(texture);
			Resources::Insert(L"Stage_king1", material);
	#pragma endregion

	#pragma region Stage2(Back)
			texture = Resources::Load<Texture>(L"King2", L"..\\Resources\\Texture\\Stage2\\King2.png");
			material = std::make_shared<Material>();
			material->SetShader(spriteShader);
			material->SetTexture(texture);
			Resources::Insert(L"Stage_king2", material);
	#pragma endregion

	#pragma region DebugShader
			std::shared_ptr<Shader> debugShader
				= Resources::Find<Shader>(L"DebugShader");

			material = std::make_shared<Material>();
			material->SetShader(debugShader);
			Resources::Insert(L"DebugMaterial", material);
#pragma endregion
	}


	void Initialize()
	{
		LoadMesh();
		LoadShader();
		LoadBuffer();
		SetupState();
		LoadMaterial();	
	}

	void renderer::PushDebugMeshAttribute(DebugMesh& mesh)
	{
		debugMeshs.push_back(mesh);
	}

	void Render()
	{
		for (Camera* cam : cameras)
		{
			if (cam == nullptr)
				continue;

			cam->Render();
		}

		cameras.clear();
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


