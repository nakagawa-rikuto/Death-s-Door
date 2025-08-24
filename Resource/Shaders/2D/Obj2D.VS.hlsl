#include "Obj2D.hlsli"

struct TransformationMatrix
{
    float4x4 WVP;
};

ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);

struct VertexShaderInput
{
    float4 position : POSITION0; // float4
    float2 texcoord : TEXCOORD0; // float2
};

VertexShaderOutput main(VertexShaderInput input)
{
    VertexShaderOutput output;
    output.position = mul(float4(input.position.xy, 0.0f, 1.0f),gTransformationMatrix.WVP);
    output.texcood = input.texcoord;
    return output;
}