// 3D�ϊ��s��
cbuffer ConstantBufferDataTransform : register(b0)
{
    matrix viewMat;
    matrix worldMat;
    float3 cameraPos;
}

// �F
cbuffer ConstantBufferDataColor : register(b2)
{
    float4 color; // �F
}