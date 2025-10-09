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
    {{ -1.0f, -1.0f },{ 0.0f, -1.0f },{ 1.0f, -1.0f}},
    {{ -1.0f, 0.0f },{ 0.0f, 0.0f },{1.0f, 0.0f }},
    {{ -1.0f, 1.0f },{ 0.0f, 1.0f},{ 1.0f,  1.0f}}
};

// 3x3の平均化フィルタ
static const float kKernel3x3[3][3] =
{
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f },
    { 1.0f / 9.0f, 1.0f / 9.0f, 1.0f / 9.0f }
};

PixelShaderOutPut main(VertexShaderOutPut input)
{
    float width, height; // 1, usvStepSizeの算出
    gTexture.GetDimensions(width, height);
    float2 usvStepSize = float2(rcp(width), rcp(height));
    
    PixelShaderOutPut output;
    output.color.rgb = float3(0.0f, 0.0f, 0.0f);
    output.color.a = 1.0f;
    for (int x = 0; x < 3; ++x) // 3x3のループ
    {
        for (int y = 0; y < 3; ++y)
        {
            // 現在のtexcordを算出
            float2 texcord = input.texcoord + kIndex3x3[x][y] * usvStepSize;
            // 色に1/9をかけて算出
            float3 fatchColor = gTexture.Sample(gSampler, texcord).rgb;
            output.color.rgb += fatchColor * kKernel3x3[x][y];

        }
    }
        return output;
}