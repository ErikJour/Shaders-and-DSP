#include "sharedShader.h"

vertex RasterizerData vertexMain(uint vertexID [[ vertex_id ]],
                                constant game_vertex *vertexArray [[ buffer(0) ]],
                                constant Uniforms &uniforms [[buffer(11)]])
{
    RasterizerData out;


    float2 pixelSpacePosition = vertexArray[vertexID].position.xy;
    float2 normalizedPosition = (pixelSpacePosition / (512.0 / 2.0)) - 1.0;

    switch (uniforms.materialId)
    {
        case MAT_BOTTOM_LEFT:
        case MAT_BOTTOM_RIGHT:
        case MAT_TOP_LEFT:
        case MAT_TOP_RIGHT:
        default:
            out.position = float4(normalizedPosition, 0.0, 1.0);
            break;
    }


    out.color    = vertexArray[vertexID].color;
    out.uv       = pixelSpacePosition / 512;

    return out;
}


