#include "Obj2D.hlsli"

struct Material
{
    float4 color;
    float4x4 uvTransform;
};

ConstantBuffer<Material> gMaterial : register(b0);
//SRVのregisterはt
Texture2D<float4> gTexture : register(t0);
// Samplerのregisterはs
SamplerState gSampler : register(s0);

struct PixlShaderOutput
{
    float4 color : SV_TARGET0;
};

PixlShaderOutput main(VertexShaderOutput input)
{
    PixlShaderOutput output;
    
    float4 transformdUV = mul(float4(input.texcood, 0.0f, 1.0f), gMaterial.uvTransform);
    
    //TextureをSamplingする
    float4 textureColor = gTexture.Sample(gSampler, transformdUV.xy);
    
    // Samplingしたtextureの色とmaterialの色を乗算して合成っしてみる
    output.color = gMaterial.color * textureColor;
    
    return output;
}