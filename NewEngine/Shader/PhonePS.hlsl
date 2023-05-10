#include "Object3D.hlsli"
#include "ShaderIO.hlsli"

Texture2D<float4> tex : register(t0); // 0番スロットに設定されたテクスチャ
SamplerState smp : register(s0); // 0番スロットに設定されたサンプラー

float4 main(VSOutputSvposPosNormalUv vsOutput) : SV_TARGET
{
    	// テクスチャーマッピング
    float4 texColor = tex.Sample(smp, vsOutput.uv);
    
    float4 shaderColor = 0;
    
    // 平行光源
    for (int i = 0; i < 1; i++)
    {
		// ライトに向かうベクトルと法線の内積
        float dotLightNormal = dot(-directionalLights[i].vec, vsOutput.normal);
        
        // 反射光ベクトル
        float3 reflectDir = normalize(directionalLights[i].vec + 2 * dotLightNormal * vsOutput.normal);
        
        // アンビエント
        float3 ambientColor = ambient * 0.3 * directionalLights[i].color;
        
        // ディフューズ
        // float intensity = smoothstep(0.45, 0.55, saturate(lightNormal));
        //float intensity = saturate(lightNormal);
        float intensity = saturate(dotLightNormal);
        float3 diffuseColor = diffuse * intensity * directionalLights[i].color;
        
        // スペキュラー
        float3 eyeDir = normalize(cameraPos.xyz - vsOutput.worldPos.xyz);
        float3 specularColor = pow(saturate(dot(reflectDir, eyeDir)), 4.0f) * specular;
        
        //shaderColor.rgb += (1 - intensity) * ambientColor + intensity * diffuseColor + specularColor;
        shaderColor.rgb += ambientColor + diffuseColor + specularColor;
    }
    
    shaderColor.a = 1;
    
    return shaderColor * texColor * color;
}