#include "Object3D.hlsli"
#include "ShaderIO.hlsli"

Texture2D<float4> tex : register(t0); // 0�ԃX���b�g�ɐݒ肳�ꂽ�e�N�X�`��
SamplerState smp : register(s0); // 0�ԃX���b�g�ɐݒ肳�ꂽ�T���v���[

float4 main(VSOutputSvposPosNormalUv vsOutput) : SV_TARGET
{
    // �e�N�X�`���[�}�b�s���O
    float4 texColor = tex.Sample(smp, vsOutput.uv);
    
    float4 shaderColor = 0;
    
    // ���s����
    for (int i = 0; i < directionalLightNum; i++)
    {
        // ���C�g�̃x�N�g��
        float3 lightDir = normalize(directionalLights[i].vec);
        
		// ���C�g�Ɍ������x�N�g���Ɩ@���̓���
        float3 lightNormal = dot(directionalLights[i].vec, vsOutput.normal);
        
        // ���ˌ��x�N�g��
        float3 reflectDir = lightDir + 2 * lightNormal * vsOutput.normal;
        
        // �A���r�G���g
        float3 ambientColor = ambient * 0.3 * directionalLights[i].color;
        
        // �f�B�t���[�Y
        float intensity = smoothstep(0.2, 0.3, lightNormal);
        float3 diffuseColor = diffuse * intensity * directionalLights[i].color;
        
        // �X�y�L�����[
        float3 eyeDir = normalize(cameraPos.xyz - vsOutput.worldPos.xyz);
        //i.normal = normalize(i.normal);

        float3 specularColor = pow(smoothstep(0.5, 0.6, dot(reflectDir, eyeDir)), 4.0f);
        
        shaderColor.rgb += ambientColor + diffuseColor + specularColor;
        
    }
    
    shaderColor.a = 1;
    
    return shaderColor * texColor * color;
}