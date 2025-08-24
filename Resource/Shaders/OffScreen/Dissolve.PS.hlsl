#include "Fullscreen.hlsli"

struct DissolveParames
{
    float threshold; // discardの閾値
    float edgeStart; // smoothstep開始
    float edgeEnd; // smoothstep終了
    float3 edgeColor; // エッジ色
    float padding; // 16バイトアライメント用
};

ConstantBuffer<DissolveParames> gDissolveParams : register(b0);

Texture2D<float4> gTexture : register(t0);
Texture2D<float> gMaskTexture : register(t1);
SamplerState gSampler : register(s0);

struct PixelShaderOutPut
{
    float4 color : SV_TARGET0;
};

PixelShaderOutPut main(VertexShaderOutPut input)
{
    float mask = gMaskTexture.Sample(gSampler, input.texcoord);

    // 動的に変更可能な threshold を使用
    if (mask <= gDissolveParams.threshold)
    {
        discard;
    }

    // エッジの滑らかさをパラメータで制御
    float edge = 1.0f - smoothstep(gDissolveParams.edgeStart, gDissolveParams.edgeEnd, mask);

    PixelShaderOutPut output;
    output.color = gTexture.Sample(gSampler, input.texcoord);

    // エッジカラーを加算
    output.color.rgb += edge * gDissolveParams.edgeColor;
    return output;
}