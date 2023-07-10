#include "jkTileMap.h"
#include "jkResources.h"
#include "jkTransform.h"
#include "jkGameObject.h"

namespace jk
{
	TileMap::TileMap()
		: BaseRenderer(eComponentType::TileMap)
		, mRowCount(0)
		, mColCount(0)
		, mTileCountX(0)
		, mTileCountY(0)
		, mbBufferUpdated(false)
		, mBuffer(nullptr)
	{
		SetName(L"TileMap Comp");

		std::shared_ptr<Material> material = Resources::Find<Material>(L"test");
		if (material)
		{
			SetMaterial(material);
			std::shared_ptr<Mesh> mesh = Resources::Find<Mesh>(L"RectMesh");
			if (mesh)
			{
				SetMesh(mesh);
			}
		}

		mTileData.resize(static_cast<size_t>(mTileCountX * mTileCountY));
		mBuffer = new StructuredBuffer;
	}

	TileMap::~TileMap()
	{
		if (nullptr != mBuffer)
			delete mBuffer;

		mBuffer = nullptr;
	}

	void TileMap::Initialize()
	{
		BaseRenderer::Initialize();
	}
	
	void TileMap::Update()
	{
		BaseRenderer::Update();
	}

	void TileMap::LateUpdate()
	{
		BaseRenderer::LateUpdate();
	}	


	void TileMap::UpdateData()
	{
		if (nullptr == mAtlasTexture)
			return;
		
		GetMaterial()->SetData(eGPUParam::Int_1, &mTileCountX);
		GetMaterial()->SetData(eGPUParam::Int_2, &mTileCountY);
		GetMaterial()->SetData(eGPUParam::Vector2_1, &mSliceUV);
		
		// ��� Ÿ�� ������(m_vecTileData) �� ����ȭ���۸� ���� t16 �������ͷ� ���ε�
		if (false == mbBufferUpdated)
		{
			mBuffer->SetData(mTileData.data(), mTileCountX * mTileCountY);
			mbBufferUpdated = true;
		}
		
		mBuffer->Bind(eShaderStage::PS, 16);
	}

	void TileMap::Render()
	{
		if (nullptr == GetMesh() || nullptr == GetMaterial() || nullptr == mAtlasTexture)
			return;

		UpdateData();

		GetOwner()->GetComponent<Transform>()->BindConstantBuffer();
		GetMesh()->BindBuffer();
		GetMaterial()->Binds();
		GetMesh()->Render();

		GetMaterial()->Clear();
	}

	void TileMap::SetTileMapCount(UINT countX, UINT countY)
	{
		mTileCountX = countX;
		mTileCountY = countY;

		ClearTileData();
	}
	
	void TileMap::SetTileData(int tileIdx, int imgIdx)
	{
		if (nullptr == mAtlasTexture)
		{
			return;
		}

		mTileData[tileIdx].mImgIdx = imgIdx;

		// ��Ʋ�󽺿��� Ÿ���� ��, �� ���� ���ϱ�
		mColCount = (UINT)mAtlasTexture->GetWidth() / (UINT)mSlicePixel.x;
		mRowCount = (UINT)mAtlasTexture->GetHeight() / (UINT)mSlicePixel.y;

		int row = mTileData[tileIdx].mImgIdx / mColCount;
		int col = mTileData[tileIdx].mImgIdx % mColCount;

		mTileData[tileIdx].mStartUV = Vector2(mSliceUV.x * col, mSliceUV.y * row);

		mbBufferUpdated = false;
	}
	
	void TileMap::ClearTileData()
	{
		std::vector<TileData> tileData;
		mTileData.swap(tileData);

		mTileData.resize(static_cast<size_t>(mTileCountX * mTileCountY));

		// ����ȭ ���۵� ũ�⿡ �����Ѵ�.
		size_t bufferSize = static_cast<size_t>(mTileCountX * mTileCountY) * sizeof(TileData);

		if (mBuffer->GetTotalSize() < bufferSize)
		{
			//UINT size, UINT stride, eSRVType type, void* data
			mBuffer->Create(sizeof(TileData), mTileCountX * mTileCountY, eSRVType::None, nullptr);
		}

		mbBufferUpdated = false;
	}

	void TileMap::SetAllTileData(int imgIdx)
	{
		size_t Size = mTileData.size();
		for (int i = 0; i < Size; ++i)
		{
			SetTileData(i, imgIdx);
		}
	}

}