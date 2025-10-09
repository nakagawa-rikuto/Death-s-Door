#include "Line3D.hlsli"

struct PixelShaderOutPut
{
    float4 color : SV_Target0;
};

PixelShaderOutPut main(VertexShaderOutput input)
{
    PixelShaderOutPut output;
    output.color = input.color;
    return output;
}