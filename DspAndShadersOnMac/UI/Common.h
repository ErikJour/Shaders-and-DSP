// Common.h
#pragma once

#ifndef __METAL_VERSION__
// C++ STL imports (ignored by MSL shader compiler)
#include <iostream>
#include <simd/simd.h>
#endif

typedef struct {
    float         audioData;
    vector_float2 viewportSize;
    float         peakX;
    uint32_t      materialId;
} Uniforms;

typedef enum :uint32_t {

    MAT_BOTTOM_LEFT  = 0,
    MAT_BOTTOM_RIGHT = 1,
    MAT_TOP_LEFT     = 2,
    MAT_TOP_RIGHT    = 3,
    MAT_COUNT
} MaterialId;

struct game_vertex {
    vector_float4 position;
    vector_float4 color;
};

struct Colors {
    vector_float4 blue  = { 0.0f, 0.0f, 1.0f, 1.0f };
    vector_float4 red   = { 1.0f, 0.0f, 0.0f, 1.0f };
    vector_float4 green = { 0.0f, 1.0f, 0.0f, 1.0f };
    vector_float4 cream = { 0.5f, 1.0f, 1.0f, 1.0f };
};

#ifndef __METAL_VERSION__
// CPU-only C++ buffer structures
struct VertexBuffer {
    game_vertex *vertices;
    uint32_t     drawCount;
};

struct GameRenderCommands {
    VertexBuffer *vertexBuffer[3];
    uint32_t      currentFrame;
};
#endif
