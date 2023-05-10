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
    for (int i = 0; i < 1; i++)
    {
		// ���C�g�Ɍ������x�N�g���Ɩ@���̓���
        float dotLightNormal = dot(-directionalLights[i].vec, vsOutput.normal);
        
        // ���ˌ��x�N�g��
        float3 reflectDir = normalize(directionalLights[i].vec + 2 * dotLightNormal * vsOutput.normal);
        
        // �A���r�G���g
        float3 ambientColor = ambient * 0.3 * directionalLights[i].color;
        
        // �f�B�t���[�Y
        // float intensity = smoothstep(0.45, 0.55, saturate(lightNormal));
        //float intensity = saturate(lightNormal);
        float intensity = saturate(dotLightNormal);
        float3 diffuseColor = diffuse * intensity * directionalLights[i].color;
        
        // �X�y�L�����[
        float3 eyeDir = normalize(cameraPos.xyz - vsOutput.worldPos.xyz);
        float3 specularColor = pow(saturate(dot(reflectDir, eyeDir)), 4.0f) * specular;
        
        //shaderColor.rgb += (1 - intensity) * ambientColor + intensity * diffuseColor + specularColor;
        shaderColor.rgb += ambientColor + diffuseColor + specularColor;
    }
    
    shaderColor.a = 1;
    
    return shaderColor * texColor * color;
}