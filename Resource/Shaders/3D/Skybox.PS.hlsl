#include "Skybox.hlsli"

struct Material
{
    float4 color;
    float4x4 uvTransform;
};

ConstantBuffer<Material> gMaterial : register(b0);
//SRVのregisterはt
TextureCube<float4> gTexture : register(t0);
// Samplerのregisterはs
SamplerState gSampler : register(s0);

struct PixlShaderOutput
{
    float4 color : SV_TARGET0;
};

PixlShaderOutput main(VertexShaderOutput input)
{
    PixlShaderOutput output;
    
    //TextureをSamplingする
    float4 textureColor = gTexture.Sample(gSampler, input.texcoord);
    
    // Samplingしたtextureの色とmaterialの色を乗算して合成っしてみる
    output.color = gMaterial.color * textureColor;
    
    return output;
}