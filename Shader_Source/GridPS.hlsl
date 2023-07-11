#include "globals.hlsli"


struct VSOut
{
    float4 Pos : SV_Position;
    float2 UV : TEXCOORD;
    float2 GridPos : POSITION;
};


float4 main(VSOut In) : SV_TARGET
{
    // �� ����
    float4 Out = ALPHA(GREY, 0.5f);

    // �밢Ÿ�� ����
    float4 OutDiagonal = ALPHA(BLUE, 0.1f);
    float4 OutInverseDiagonal = ALPHA(YELLOW, 0.1f);

    // Ÿ���� ������ �� ĭ�� ũ�⸦ ��Ÿ����.
    // 33�� 100�� ���εȴ�.  
    const float width = 50.f;//42.6f;
    const float height = 50.f;//42.6f;

    // ���� ������ 329.9f ����
    const float widthX10 = 500.f;//426.f;
    const float heightX10 = 500.f;//426.f;

    // �ȼ��� ���� ��ǥ
    int gridX = (int)In.GridPos.x;
    int gridY = (int)In.GridPos.y;

    // �� �β�
    const float thickness = 1.f * cameraScale.x;

    // �� ĭ ���� �ȼ��� ��ǥ
    float calcX = abs((float)(gridX) % width);
    float calcY = abs((float)(gridY) % height);

    float calcX10 = abs((float)(gridX) % widthX10);
    float calcY10 = abs((float)(gridY) % heightX10);

    // ������ ���� �׸���
    if (calcX <= thickness || calcY <= thickness)
    {
        float4 axisXColor = BLACK;
        float4 axisYColor = BLACK; 

        // ��ǥ�� �׸���
        if (abs(gridX) <= thickness)
            return axisYColor;
        if (abs(gridY) <= thickness)
            return axisXColor;

        if (abs(gridX) <= thickness * 3.f)
            return axisYColor;
        if (abs(gridY) <= thickness * 3.f)
            return axisXColor;

        if (abs(calcX10) <= thickness || abs(calcY10) <= thickness)
            return RED;

        return Out;
    }

    // Ÿ���� ũ��
    const int halfWidth = width * 0.5f;
    const int halfHeight = height * 0.5f;

    // Ÿ�� ĥ�ϱ�
    if (calcX < halfWidth && calcY < halfHeight
        || calcX > halfWidth && calcY > halfHeight)
    {
        // 1��и�
        if (gridY > 0.f)
        {
            if (gridX > 0.f)
                return OutDiagonal;
            else if (gridX < 0.f)
                return OutInverseDiagonal;
        }
        // 2��и�
        else if (gridY < 0.f)
        {
            if (gridX > 0.f)
                return OutInverseDiagonal;
            else if (gridX < 0.f)
                return OutDiagonal;
        }
    }

    if (gridY > 0.f)
    {
        // 3��и�
        if (gridX > 0.f)
            return OutInverseDiagonal;
        else if (gridX < 0.f)
            return OutDiagonal;
    }
    else
    {
        // 4��и�
        if (gridX > 0.f)
            return OutDiagonal;
        else if (gridX < 0.f)
            return OutInverseDiagonal;
    }

    return float4(1.f, 0.f, 1.f, 1.f);
}