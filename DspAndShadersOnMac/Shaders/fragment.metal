#include "sharedShader.h"
#include "squareShaders.h"


fragment float4 fragmentMain(RasterizerData in [[stage_in]],
                                constant Uniforms &uniforms [[buffer(11)]])
{
    if (uniforms.materialId == MAT_BOTTOM_LEFT)  { return fragmentBottomLeft  (in, uniforms); }
    if (uniforms.materialId == MAT_BOTTOM_RIGHT) { return fragmentBottomRight (in, uniforms); }
    if (uniforms.materialId == MAT_TOP_LEFT)     { return fragmentTopLeft     (in, uniforms); }
    if (uniforms.materialId == MAT_TOP_RIGHT)    { return fragmentTopRight    (in, uniforms); }
    
    return in.color;
}
