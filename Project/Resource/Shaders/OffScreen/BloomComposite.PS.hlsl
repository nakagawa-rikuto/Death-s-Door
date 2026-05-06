#include "Fullscreen.hlsli"

struct BloomParams
{
    float threshold;
    float3 brightness;
    float intensity;
    float3 padding;
};

ConstantBuffer<BloomParams> gBloomCompositeParams : register(b0);
// t0: 元のシーンのテクスチャ
Texture2D<float4> gMainTexture : register(t0);
// t1: 輝度抽出＋ブラーをかけたテクスチャ
Texture2D<float4> gBlurTexture : register(t1);

SamplerState gSampler : register(s0);

struct PixelShaderOutPut
{
    float4 color : SV_TARGET0;
};

PixelShaderOutPut main(VertexShaderOutPut input)
{
    PixelShaderOutPut output;
    
    float4 mainColor = gMainTexture.Sample(gSampler, input.texcoord);
    float4 blurColor = gBlurTexture.Sample(gSampler, input.texcoord);
    
    // 加算合成（ブルームの強度を変えたい場合は blurColor に係数を掛ける）
        output.color = mainColor + (blurColor * gBloomCompositeParams.intensity);
    output.color.a = 1.0f;
    
    return output;
}