//
// Created by Erik Jourgensen on 10/30/26.
//

#pragma once
#include "../sharedShader.h"

inline float4 fragmentButton(RasterizerData in)
{
    // float lum   = logoTex.sample(logoSmp, in.uv).r;
    // float alpha = smoothstep(0.15, 0.65, lum) * 0.75;
    return float4(1.0);
}

inline float4 vertexLogo(float3 pos, constant Uniforms &u)
{
return u.projMatrix * u.modelMatrix * float4(pos, 1.0);
}
