#pragma once
#include <metal_stdlib>
#include "../UI/Common.h"

using namespace metal;
#define PI 3.14159265359

typedef struct {
    float4 position [[position]];
    float4 color;
    float2 uv;
} RasterizerData;

