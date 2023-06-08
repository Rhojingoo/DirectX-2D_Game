#pragma once
#include "JKEngine.h"
#include "jkGraphicDevice_Dx11.h"
#include "jkMesh.h"
#include "jkShader.h"

using namespace jk::math;
namespace jk::renderer
{
	struct Vertex
	{
		Vector3 pos;
		Vector4 color;
	};

	extern Vertex vertexes[];
	extern jk::Mesh* mesh;
	extern ID3D11Buffer* triangleConstantBuffer;
	extern jk::Shader* shader;


	void Initialize();
	void Release();
}


