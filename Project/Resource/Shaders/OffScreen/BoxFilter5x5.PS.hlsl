#include "Fullscreen.hlsli"

Texture2D<float4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutPut {
    float4 color : SV_TARGET0;
};

// ガウシアンフィルタ用のサンプルオフセット
static const float2 kIndex5x5[5][5] = {
    { {-2.0f, -2.0f}, {-1.0f, -2.0f}, {0.0f, -2.0f}, {1.0f, -2.0f}, {2.0f, -2.0f} },
    { {-2.0f, -1.0f}, {-1.0f, -1.0f}, {0.0f, -1.0f}, {1.0f, -1.0f}, {2.0f, -1.0f} },
    { {-2.0f,  0.0f}, {-1.0f,  0.0f}, {0.0f,  0.0f}, {1.0f,  0.0f}, {2.0f,  0.0f} },
    { {-2.0f,  1.0f}, {-1.0f,  1.0f}, {0.0f,  1.0f}, {1.0f,  1.0f}, {2.0f,  1.0f} },
    { {-2.0f,  2.0f}, {-1.0f,  2.0f}, {0.0f,  2.0f}, {1.0f,  2.0f}, {2.0f,  2.0f} }
};

// 正規化された5x5ガウシアンカーネル（合計 = 1）
static const float kGaussianKernel5x5[5][5] = {
    { 1.0 / 256.0,  4.0 / 256.0,  6.0 / 256.0,  4.0 / 256.0,  1.0 / 256.0 },
    { 4.0 / 256.0, 16.0 / 256.0, 24.0 / 256.0, 16.0 / 256.0,  4.0 / 256.0 },
    { 6.0 / 256.0, 24.0 / 256.0, 36.0 / 256.0, 24.0 / 256.0,  6.0 / 256.0 },
    { 4.0 / 256.0, 16.0 / 256.0, 24.0 / 256.0, 16.0 / 256.0,  4.0 / 256.0 },
    { 1.0 / 256.0,  4.0 / 256.0,  6.0 / 256.0,  4.0 / 256.0,  1.0 / 256.0 }
};

PixelShaderOutPut main(VertexShaderOutPut input) {
    float width, height;
    gTexture.GetDimensions(width, height);
    float2 usvStepSize = float2(rcp(width), rcp(height));

    PixelShaderOutPut output;
    output.color.rgb = float3(0.0f, 0.0f, 0.0f);
    output.color.a = 1.0f;

    // 5x5 ガウシアンブラーの適用
    for (int x = 0; x < 5; ++x) 
    {
        for (int y = 0; y < 5; ++y) 
        {
            float2 texcoord = input.texcoord + kIndex5x5[x][y] * usvStepSize;
            float3 fetchColor = gTexture.Sample(gSampler, texcoord).rgb;
            output.color.rgb += fetchColor * kGaussianKernel5x5[x][y];
        }
    }

    return output;
}