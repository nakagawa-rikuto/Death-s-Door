#include "Fullscreen.hlsli"

// 抽出する明るさの閾値を定数バッファで制御
struct BloomParams
{
    float threshold;
    float3 brightness;
    float intensity;
    float3 padding;
};

ConstantBuffer<BloomParams> gBloomParams : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutPut
{
    float4 color : SV_TARGET0;
};

PixelShaderOutPut main(VertexShaderOutPut input)
{
    PixelShaderOutPut output;
    float4 color = gTexture.Sample(gSampler, input.texcoord);
    
    // RGBの輝度（明るさ）を計算(後でこの部分の実数値を変更できるようにする)
    float luminance = dot(color.rgb, gBloomParams.brightness);
    
    // 閾値を超えたものだけを残し、暗い部分は黒（0）にする
    if (luminance >= gBloomParams.threshold)
    {
        output.color = color;
    } 
    else
    {
        output.color = float4(0.0f, 0.0f, 0.0f, 1.0f);
    }
    
    return output;
}