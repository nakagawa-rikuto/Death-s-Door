#include "Fullscreen.hlsli"

struct RadialBlurParameters
{
    float2 center; // 中心点
    int numSamples; // サンプリング数
    float blurWidth; // ブラーの幅
    float padding; // HLSL の16バイト境界合わせ用
};

ConstantBuffer<RadialBlurParameters> gRadialBlurParams : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutPut
{
    float4 color : SV_TARGET0;
};

PixelShaderOutPut main(VertexShaderOutPut input)
{   
    // 中心点。ここを基準に放射状にブラーがかかる
    const float2 kCenter = gRadialBlurParams.center;
    // サンプリング数（多いほど滑らかが思い）
    const int kNumSamples = gRadialBlurParams.numSamples;
    // ぼかしの幅 (大きい程、おおきい)
    const float kBlurWidth = gRadialBlurParams.blurWidth;

    // 中心から現在のuvに対しての方向を計算
    float2 direction = input.texcoord - kCenter;
    float3 outputColor = float3(0.0f, 0.0f, 0.0f);
    for (int samplerIndex = 0; samplerIndex < kNumSamples; ++samplerIndex)
    {
        // 現在のuvから先程計算した方向にサンプリング店を進めながらサンプリングしていく
        float2 texcoord = input.texcoord + direction * kBlurWidth * float(samplerIndex);
        outputColor += gTexture.Sample(gSampler, texcoord).rgb;
    }
    
    // 平均化する
    outputColor.rgb *= rcp(float(kNumSamples));
    
    PixelShaderOutPut output;
    output.color.rgb = outputColor;
    output.color.a = 1.0f;

    return output;
}