//
// Created by Erik Jourgensen on 9/22/26.
//
#import "../Geometries/SquareGeo.h"
#import "../Geometries/TriangleGeo.h"
#import "Common.h"
#import "../Audio/AudioTypes.h"
#include "mainWindow.h"
#include <algorithm>

#define GLOBAL_WIDTH  512
#define GLOBAL_HEIGHT 512
static const NSUInteger kMaxBuffers = 3;
static vector_float4 buttonColor     = { 1.0f, 0.0f, 0.0f, 1.0f };
//==============================================================
@interface
MTKViewDelegate: NSObject <MTKViewDelegate>
@property (retain) id<MTLCommandQueue>        commandQueue;
@property (retain) NSMutableArray*            macVertexBuffers;
@property (retain) id<MTLRenderPipelineState> pipelineState;
@property GameRenderCommands                  gameRenderCommands;
@property Uniforms                            uniforms;
@property (nonatomic, assign) MyRenderer      *audioRenderer;
@property UInt32                              x;
@property Colors                              colors;
@end

//==============================================================
@implementation MTKViewDelegate
{
    dispatch_semaphore_t     _frameBoundarySemaphore;
    uint32_t      			 _currentFrameIndex;
}

-(void)configureMetal
{
    _frameBoundarySemaphore = dispatch_semaphore_create(kMaxBuffers);
    _currentFrameIndex      = 0;
}

-(void)mtkView:(MTKView *) view drawableSizeWillChange:(CGSize) size  {}

- (void)setXOffset:(float)amount {}

- (void)drawInMTKView:(MTKView *) view
{
    dispatch_semaphore_wait(_frameBoundarySemaphore, DISPATCH_TIME_FOREVER);

    uint32_t frameIndex   = _currentFrameIndex;
    uint32_t x = _x;

    uint32_t vertexCount = 0;

    game_vertex *vertices = self.gameRenderCommands.vertexBuffer[frameIndex]->vertices;

    vector_float4 blue      = { 0.0f, 0.0f, 1.0f, 1.0f };
    vector_float4 red       = { 1.0f, 0.0f, 0.0f, 1.0f };
    vector_float4 green     = { 0.0f, 1.0f, 0.0f, 1.0f };
    vector_float4 magenta   = { 1.0f, 0.0f, 1.0f, 1.0f };

    //Bottom left
    uint32_t squareAStart = vertexCount;
    drawSquare(vertices, &vertexCount, 128, blue, -128, -128);
    uint32_t squareACount = vertexCount - squareAStart;
    //Bottom right
    uint32_t squareBStart = vertexCount;
    drawSquare(vertices, &vertexCount, 128, red,   128,  -128);
    uint32_t squareBCount = vertexCount - squareBStart;
    //Top left
    uint32_t squareCStart = vertexCount;
    drawSquare(vertices, &vertexCount, 128, green,   -128,  128);
    uint32_t squareCCount = vertexCount - squareCStart;
    //Top right
    uint32_t squareDStart = vertexCount;
    drawSquare(vertices, &vertexCount, 128, magenta,   128,  128);
    uint32_t squareDCount = vertexCount - squareDStart;


    @autoreleasepool{

        id<MTLCommandBuffer> CommandBuffer = [self.commandQueue commandBuffer];

        MTLRenderPassDescriptor *RenderPassDescriptor       = [view currentRenderPassDescriptor];
        RenderPassDescriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
        RenderPassDescriptor.colorAttachments[0].clearColor = MTLClearColorMake(0.1f, 0.1f, 0.1f, 1.0f);

        id<MTLRenderCommandEncoder> RenderEncoder = [CommandBuffer renderCommandEncoderWithDescriptor:RenderPassDescriptor];
        RenderEncoder.label = @"RenderEncoder";

        // Viewport using actual drawable size
        CGSize drawableSize = view.drawableSize;
        MTLViewport viewPort = { 0.0, 0.0, drawableSize.width, drawableSize.height, 0.0, 1.0 };
        [RenderEncoder setViewport: viewPort];

        [RenderEncoder setRenderPipelineState: [self pipelineState]];
        [RenderEncoder setVertexBuffer: [[self macVertexBuffers] objectAtIndex: _currentFrameIndex]
                                offset: 0
                                 atIndex: 0];

        // ==============================================
        // PASS UNIFORMS TO FRAGMENT SHADER AT INDEX 11
        // ==============================================
        _uniforms.audioData     = _audioRenderer->data.outputData;
        _uniforms.viewportSize = (vector_float2){ 512.0f, 512.0f };

        //Bottom Left Square
        _uniforms.materialId = MAT_BOTTOM_LEFT;
        [RenderEncoder setVertexBytes:&_uniforms   length:sizeof(Uniforms) atIndex:11];
        [RenderEncoder setFragmentBytes:&_uniforms length:sizeof(Uniforms) atIndex:11];
        [RenderEncoder drawPrimitives:MTLPrimitiveTypeTriangle
                          vertexStart:squareAStart
                          vertexCount:squareACount];

        //Bottom right square
        _uniforms.materialId = MAT_BOTTOM_RIGHT;
        [RenderEncoder setVertexBytes:&_uniforms   length:sizeof(Uniforms) atIndex:11];
        [RenderEncoder setFragmentBytes:&_uniforms length:sizeof(Uniforms) atIndex:11];
        [RenderEncoder drawPrimitives:MTLPrimitiveTypeTriangle
                          vertexStart:squareBStart
                          vertexCount:squareBCount];

        //Top left
        _uniforms.materialId = MAT_TOP_LEFT;
        [RenderEncoder setVertexBytes:&_uniforms   length:sizeof(Uniforms) atIndex:11];
        [RenderEncoder setFragmentBytes:&_uniforms length:sizeof(Uniforms) atIndex:11];
        [RenderEncoder drawPrimitives:MTLPrimitiveTypeTriangle
                          vertexStart:squareCStart
                          vertexCount:squareCCount];
//        //Top right
        _uniforms.materialId = MAT_TOP_RIGHT;
        [RenderEncoder setVertexBytes:&_uniforms   length:sizeof(Uniforms) atIndex:11];
        [RenderEncoder setFragmentBytes:&_uniforms length:sizeof(Uniforms) atIndex:11];
        [RenderEncoder drawPrimitives:MTLPrimitiveTypeTriangle
                          vertexStart:squareDStart
                          vertexCount:squareDCount];

        //==================================================================================


        [RenderEncoder endEncoding];

        id<CAMetalDrawable> NextDrawable = [view currentDrawable];
        [CommandBuffer presentDrawable:NextDrawable];

        uint32_t nextIndex = _currentFrameIndex + 1;
        if (nextIndex > 2) {
            nextIndex = 0;
        }

        _currentFrameIndex = nextIndex;

        __block dispatch_semaphore_t semaphore = _frameBoundarySemaphore;

        [CommandBuffer addCompletedHandler:^(id<MTLCommandBuffer> commandBuffer) {
            dispatch_semaphore_signal(semaphore);
        }];

        [CommandBuffer commit];
    }
}

@end