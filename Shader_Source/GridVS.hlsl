#include "globals.hlsli"

//struct VSIn
//{
//    float3 Pos : POSITION;
//    float2 UV : TEXCOORD;
//};
//
//struct VSOut
//{
//    float4 Pos : SV_Position;
//    float2 UV : TEXCOORD;
//    float2 GridPos : POSITION;
//};
//
//VSOut main(VSIn In) 
//{
//    //���ڵ�
//    VSOut Out = (VSOut) 0.0f;
//    float2 gridPos = float2(0.0f, 0.0f);
//    gridPos.x = In.Pos.x * 1.0f * Resolution.x + CameraPosition.x * CameraScale.x;
//    gridPos.y = In.Pos.y * 1.0f * Resolution.y + CameraPosition.y * CameraScale.y;
//    const float meshScale = 2.0f;
//    Out.Pos = float4(In.Pos.xy * meshScale, 0.999f, 1.0f);
//    Out.UV = In.UV;
//    Out.GridPos = gridPos;    
//    return Out;
//}


struct VSIn
{
    float4 Pos : POSITION;
    float2 UV : TEXCOORD;
};

struct VSOut
{
    float4 Pos : SV_POSITION;
    float2 UV : TEXCOORD;

    float2 WorldPos : POSITION;
};

VSOut main(VSIn In)
{
    VSOut Out = (VSOut)0.f;

    // �׷��� �޽��� ũ��� ���� ī�޶��� ������ ����Ͽ� ȭ�鿡 ���������� �׷��� ũ�⸦ ���س���.
    float calcMeshScale = meshScale + cameraScale.x;

    // �޽��� ����� ��ġ�� ī�޶��� �̵��� ���� ������ �޾� �׷�����.
    float2 worldPos = float2(0.f, 0.f);
    // �׸����� ��ġ�� ī�޶� ������ �ػ󵵸� �����Ѵ�.
    // ī�޶��� �̵����� ���������� �׷��� ũ�⸸ŭ�� ������ �����ؾ� �Ѵ�.
    // 2.f�� ������ ������ meshScale�� 0.5��� ����Ǳ� ������ �̸� �ǵ����� ���ؼ�.
    worldPos.x = In.Pos.x * cameraScale.x * resolution.x + (cameraPosition.x / calcMeshScale * 2.f);
    worldPos.y = In.Pos.y * cameraScale.y * resolution.y + (cameraPosition.y / calcMeshScale * 2.f);

    // PS�� �Ѱ��ֱ� ���� ���� ���ؽ� ������ �׷��� ũ�⸦ �ݿ��Ѵ�.
    // z���� w���� �Ʒ��� ���� ������ ȭ�� ���� �ڿ��� �׷��ֱ� ���ؼ��̴�.
    Out.Pos = float4(In.Pos.xy * calcMeshScale, 0.999f, 1.f);
    Out.UV = In.UV;
    Out.WorldPos = worldPos;

    return Out;
}