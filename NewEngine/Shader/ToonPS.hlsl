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
    for (int i = 0; i < directionalLightNum; i++)
    {
        // ライトのベクトル
        float3 lightDir = normalize(directionalLights[i].vec);
        
		// ライトに向かうベクトルと法線の内積
        float3 lightNormal = dot(directionalLights[i].vec, vsOutput.normal);
        
        // 反射光ベクトル
        float3 reflectDir = lightDir + 2 * lightNormal * vsOutput.normal;
        
        // アンビエント
        float3 ambientColor = ambient * 0.3 * directionalLights[i].color;
        
        // ディフューズ
        float intensity = smoothstep(0.2, 0.3, lightNormal);
        float3 diffuseColor = diffuse * intensity * directionalLights[i].color;
        
        // スペキュラー
        float3 eyeDir = normalize(cameraPos.xyz - vsOutput.worldPos.xyz);
        //i.normal = normalize(i.normal);

        float3 specularColor = pow(smoothstep(0.5, 0.6, dot(reflectDir, eyeDir)), 4.0f);
        
        shaderColor.rgb += ambientColor + diffuseColor + specularColor;
        
    }
    
    shaderColor.a = 1;
    
    return shaderColor * texColor * color;
}