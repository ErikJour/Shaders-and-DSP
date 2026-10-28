// shaders.metal
#include <metal_stdlib>
#include "../UI/Common.h"

using namespace metal;
#define PI 3.14159265359

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

float plot (float2 st, float pct, float data)
{
    data = (data * 0.01) + 0.01;
    return smoothstep(pct - 0.01, pct, st.y) - smoothstep(pct, pct + 0.01, st.y) ;
}



fragment float4 fragmentMain(
        RasterizerData in [[stage_in]],
constant Uniforms &uniforms [[buffer(11)]])
{

float3 colorA = float3(0.149, 0.141, 0.912);
float3 colorB = float3(1.000, 0.833, 0.224);

float3 color = float3(0.0);
float pct = abs(sin(uniforms.audioData));
color = mix(colorA, colorB, pct);
return float4(color, 1.0);
}