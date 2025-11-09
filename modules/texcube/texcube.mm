#include "metal-types.h"
#import <merc/module.h>
#import <Metal/Metal.h>
#import <simd/simd.h>

@interface MercTestTriangle : NSObject
- (id)init:(id<MTLDevice>)device colorPixelFormat:(MTLPixelFormat)f sampleCount:(NSUInteger)s;
- (void)draw:(id<MTLRenderCommandEncoder>)renderEncoder;
@end

@implementation MercTestTriangle
{
    id<MTLRenderPipelineState> pipelineState;
}

- (id)init:(id<MTLDevice>)device colorPixelFormat:(MTLPixelFormat)f sampleCount:(NSUInteger)s
{
    self = [super init];
    if (!self) return nil;

    NSError *error;
    id<MTLLibrary> defaultLibrary = [device newDefaultLibraryWithBundle:[NSBundle bundleForClass:self.class] error:&error];
    NSAssert(defaultLibrary, @"Failed to create shader library: %@", error);
    id<MTLFunction> vertexFunction = [defaultLibrary newFunctionWithName:@"vertexShader"];
    id<MTLFunction> fragmentFunction = [defaultLibrary newFunctionWithName:@"fragmentShader"];
    MTLRenderPipelineDescriptor* pipelineStateDescriptor = [MTLRenderPipelineDescriptor new];
    pipelineStateDescriptor.label = @"Triangle Pipeline";
    pipelineStateDescriptor.vertexFunction = vertexFunction;
    pipelineStateDescriptor.fragmentFunction = fragmentFunction;
    pipelineStateDescriptor.colorAttachments[0].pixelFormat = f;
    pipelineStateDescriptor.rasterSampleCount = s;
    pipelineState = [device newRenderPipelineStateWithDescriptor:pipelineStateDescriptor
                                                           error:&error];
    NSAssert(pipelineState, @"Failed to create pipeline state: %@", error);
    return self;
}

- (void)draw:(id<MTLRenderCommandEncoder>)renderEncoder
{
    static const Vertex triangleVertices[] =
    {
        // 2D positions,    RGBA colors
        { {  0.25,  -0.25 }, { 1, 1, 1, 1 } },
        { { -0.25,  -0.25 }, { 0, 1, 1, 1 } },
        { {     0,   0.25 }, { 1, 1, 1, 1 } },
    };
 
    [renderEncoder setRenderPipelineState:pipelineState];

    // Pass in the parameter data.
    [renderEncoder setVertexBytes:triangleVertices
                           length:sizeof(triangleVertices)
                          atIndex:0];

    // Draw the triangle.
    [renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle
                      vertexStart:0
                      vertexCount:3];

    [renderEncoder endEncoding];
}
@end

struct Triangle : merc::VideoEffect
{
    static const char* getCategory() { return kVstVideoEffectClass; }
    static const char* getSubCategory() { return merc::videoSubCategory::frameBuffer; }
    static const char* getName() { return "triangle"; }
    Triangle()
    {
        mercTestTriangle = [MercTestTriangle alloc];
    }
    Steinberg::tresult PLUGIN_API setup(id<MTLDevice> device, MTLPixelFormat pixelFormat, NSUInteger sampleCount) override
    {
        [mercTestTriangle init:device colorPixelFormat:pixelFormat sampleCount:sampleCount];
        return Steinberg::kResultOk;
    }
    Steinberg::tresult PLUGIN_API draw(id<MTLRenderCommandEncoder> renderEncoder) override
    {
        [mercTestTriangle draw:renderEncoder];
        return Steinberg::kResultOk;
    }

    MercTestTriangle __strong* mercTestTriangle;
};

template struct merc::Module<Triangle>;
