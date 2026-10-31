// shaders.metal
#include <metal_stdlib>
#include <metal_logging>
#include "../UI/Common.h"

using namespace metal;
#define PI 3.14159265359

typedef struct {
    float4 position [[position]];
    float4 color;
    float2 uv;
} RasterizerData;

vertex RasterizerData vertexMain(uint vertexID [[ vertex_id ]],
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
    return smoothstep(pct - 0.01, pct, st.y) - smoothstep(pct, pct + 0.01, st.y) ;
}


fragment float4 fragmentMain(RasterizerData in [[stage_in]],
                                constant Uniforms &uniforms [[buffer(11)]])
{
    float data    = uniforms.audioData;
    float3 colorA = float3(0.149,0.141,0.912);
    float3 colorB = float3(1.000,0.833,0.224);
    float2 st     = in.uv;
    float3 color  = float3(0.0);
    float3 pct    = float3(st.x);

    pct.r = smoothstep(0.0,1.0, st.x * data);
    pct.g = sin(st.x*PI * data);
    pct.b = pow(st.x, 0.5);
    color = mix(colorA, colorB, pct);
    //====================================================
    // Plot transition lines for each channel
    //====================================================
    //color = mix(color,float3(1.0,0.0,0.0),plot(st,pct.r));
    //color = mix(color,float3(0.0,1.0,0.0),plot(st,pct.g));
    //color = mix(color,float3(0.0,0.0,1.0),plot(st,pct.b));

    return float4(color, 1.0);
}
