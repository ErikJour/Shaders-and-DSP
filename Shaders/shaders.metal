// shaders.metal
#include <metal_stdlib>
#include "../UI/Common.h"

using namespace metal;

typedef struct {
    float4 position [[position]];
    float4 color;
    float2 uv;
} RasterizerData;

vertex RasterizerData

vertexMain(uint vertexID [[ vertex_id ]],
           constant game_vertex *vertexArray [[ buffer(0) ]])
{
    RasterizerData out;
    float2 pixelSpacePosition = vertexArray[vertexID].position.xy;
    float2 normalizedPosition = (pixelSpacePosition / (512.0 / 2.0)) - 1.0;

    out.position = float4(normalizedPosition.x, normalizedPosition.y, 0.0, 1.0);
    out.color    = vertexArray[vertexID].color;
    out.uv       = pixelSpacePosition / 512;
    return out;
}

float plot (float2 st, float pct)
{
    return smoothstep(pct - 0.02, pct, st.y) - smoothstep(pct, pct + 0.02, st.y) ;
}

fragment float4 fragmentMain(
        RasterizerData in [[stage_in]],
        constant Uniforms &uniforms [[buffer(11)]])
        {

        float2 st    = in.uv;
        float y = smoothstep(0.2, 0.5, st.x) - smoothstep(0.5, 0.8, st.x);
        float3 color = float3(y);
        float pct = plot(st, y);

        color = (1.0 - pct) * color + pct * float3(0.0, 1.0, 0.0);
        return float4(color, 1.0);
        }