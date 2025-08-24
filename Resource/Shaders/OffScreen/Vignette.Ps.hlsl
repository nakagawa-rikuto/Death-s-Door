#include "Fullscreen.hlsli"

struct VignetteParames
{
    float scale; // vignetteのスケール
    float pawer; // vignetteのパワー
    float2 padding; // 16バイトアライメント用
};

ConstantBuffer<VignetteParames> gVignetteParams : register(b0);
Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutPut
{
    float4 color : SV_TARGET0;
};

PixelShaderOutPut main(VertexShaderOutPut input)
{
    PixelShaderOutPut output;
    output.color = gTexture.Sample(gSampler, input.texcoord);
    
    // 周囲を0に、中心になるほど明るくなるように計算で調整
    float2 correct = input.texcoord * (1.0f - input.texcoord.yx);
    // correctだけで計算すると中心の最大値が0.00625で暗すぎるのでScaleで調整
    float vignette = correct.x * correct.y * gVignetteParams.scale;
    // とりあえず0.8上でそれっぽくする
    vignette = saturate(pow(vignette, gVignetteParams.pawer));
    // 係数として乗算
    output.color.rgb *= vignette;
    
    return output;
}