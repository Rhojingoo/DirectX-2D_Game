#pragma once
#include "JKEngine.h"
#include "jkMath.h"
#include "jkGraphicDevice_Dx11.h"
#include "jkMesh.h"
#include "jkShader.h"
#include "jkConstantBuffer.h"
#include "jkCamera.h"
#include "jkStructuredBuffer.h"

using namespace jk::math;
using namespace jk::graphics;
namespace jk::renderer
{
	struct Vertex
	{
		Vector3 pos;
		Vector4 color;
		Vector2 uv;
	};

	CBUFFER(TransformCB, CBSLOT_TRANSFORM)
	{
		Matrix world;
		Matrix view;
		Matrix projection;
	};

	CBUFFER(MaterialCB, CBSLOT_MATERIAL)
	{
		int iData1;
		int iData2;
		int iData3;
		int iData4;

		float fData1;
		float fData2;
		float fData3;
		float fData4;

		Vector2 xy1;
		Vector2 xy2;
		Vector2 xy3;
		Vector2 xy4;

		Vector3 xyz1;
		float	xyzPadding1;
		Vector3 xyz2;
		float	xyzPadding2;
		Vector3 xyz3;
		float	xyzPadding3;
		Vector3 xyz4;
		float	xyzPadding4;

		Vector4 xyzw1;
		Vector4 xyzw2;
		Vector4 xyzw3;
		Vector4 xyzw4;

		Matrix matrix1;
		Matrix matrix2;
		Matrix matrix3;
		Matrix matrix4;
	};


	CBUFFER(GridCB, CBSLOT_GRID)
	{
		Vector4 cameraPosition;
		Vector2 cameraScale;
		Vector2 resolution;
		float	meshScale;
		Vector3 padding;

		//Vector4 CameraPosition;
		//Vector2 CameraScale;
		//Vector2 Resolution;
	};

	CBUFFER(ObjectTypeMoveCB, CBSLOT_MOVE)
	{
		int mtype;
		Vector3 mTime;
	};

	extern jk::graphics::ConstantBuffer* constantBuffer[(UINT)eCBType::End];


	extern Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerState[];
	extern Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStates[];
	extern Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilStates[];
	extern Microsoft::WRL::ComPtr<ID3D11BlendState> blendStates[];

	extern std::vector<jk::Camera*> cameras;
	extern std::vector<DebugMesh> debugMeshs;

	void Initialize();
	void Render();
	void Release();

	void PushDebugMeshAttribute(DebugMesh& mesh);
}


