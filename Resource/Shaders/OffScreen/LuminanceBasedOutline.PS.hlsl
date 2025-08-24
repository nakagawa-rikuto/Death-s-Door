#include "Fullscreen.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutPut
{
    float4 color : SV_TARGET0;
};

// 3x3のフィルタを適用するためのテクスチャ座標のオフセット
static const float2 kIndex3x3[3][3] =
{
    { { -1.0f, -1.0f }, { 0.0f, -1.0f }, { 1.0f, -1.0f } },
    { { -1.0f, 0.0f }, { 0.0f, 0.0f }, { 1.0f, 0.0f } },
    { { -1.0f, 1.0f }, { 0.0f, 1.0f }, { 1.0f, 1.0f } }
};

// 3x3の平均化フィルタ
static const float kKernel3x3[3][3] =
{
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f }
};

// 
static const float kPrewittHorizontalKernel[3][3] =
{
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f },
    { -1.0f / 6.0f, 0.0f, 1.0f / 6.0f }
};

// 
static const float kPrewittVerticalKernel[3][3] =
{
    { -1.0f / 6.0f, -1.0f / 6.0f, -1.0f / 6.0f },
    { 0.0f, 0.0f, 0.0f },
    { 1.0f / 6.0f, 1.0f / 6.0f, 1.0f / 6.0f }
};

// RGBの輝度を算出するための係数
float Luminance(float3 v)
{
    return dot(v, float3(0.2125f, 0.7154f, 0.0721f));
}

PixelShaderOutPut main(VertexShaderOutPut input)
{
    float width, height; // 1, usvStepSizeの算出
    gTexture.GetDimensions(width, height);
    float2 usvStepSize = float2(rcp(width), rcp(height));
    float2 difference = float2(0.0f, 0.0f);
    
    
    for (int x = 0; x < 3; ++x) // 3x3のループ
    {
        for (int y = 0; y < 3; ++y)
        {
            // 現在のtexcordを算出
            float2 texcord = input.texcoord + kIndex3x3[x][y] * usvStepSize;
            // 色に1/9をかけて算出
            float3 fatchColor = gTexture.Sample(gSampler, texcord).rgb;
            float luminance = Luminance(fatchColor);
            difference.x += luminance * kPrewittHorizontalKernel[x][y];
            difference.y += luminance * kPrewittVerticalKernel[x][y];
        }
    }
    
    // 変化の長さをウェイトとして合成。ウェイトの決定方法も色々と考えられる。例えばdifference.xだけ使えば横方向のエッジが検出される
    float weigtht = length(difference);
    weigtht = saturate(weigtht);
    // 差が小さすぎてわかりづらいので適当に6倍している。CBufferで調整パラメータとして送ったりするといい
    PixelShaderOutPut output;
    // weightが大きいほど暗くするようにしている。最もシンプルな合成方法
    output.color.rgb = (1.0f - weigtht) * gTexture.Sample(gSampler, input.texcoord).rgb;
    output.color.a = 1.0f;
    
    
    return output;
}