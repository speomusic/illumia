#import <Metal/Metal.h>
#import <simd/simd.h>
#import <merc/vst/module.h>
#import <merc/vst/graphics.h>
#import <merc/vst/voice-pool.h>
#include "metal-types.h"

@class MercTriangle;

struct TriangleVoice
{
    using Context = MercTriangle;
    void start(MercTriangle* context, const Steinberg::Vst::Event& event);
    void play(MercTriangle* context);
    void stop(MercTriangle* context, const Steinberg::Vst::Event& event);
};

@interface MercTriangle : NSObject
- (id)init;
- (void)setArrangement:(merc::vst::VideoBusArrangement)arrangement
              graphics:(const merc::vst::Graphics&)graphics;
- (void)process:(const merc::vst::Graphics&)graphics
           data:(merc::vst::VideoData&)data;
@property id<MTLRenderCommandEncoder> renderEncoder;
@end

@implementation MercTriangle
{
    id<MTLRenderPipelineState> pipelineState;
    id<MTLTexture> multisampleTexture;
    std::unique_ptr<merc::vst::VoicePool<TriangleVoice>> voicePool;
}

- (id)init
{
    self = [super init];
    if (!self) return nil;

    voicePool = std::make_unique<merc::vst::VoicePool<TriangleVoice>>(128);
    return self;
}

- (void)setArrangement:(merc::vst::VideoBusArrangement)arrangement
              graphics:(const merc::vst::Graphics&)graphics
{
    NSError *error;
    id<MTLLibrary> defaultLibrary = [graphics.device newDefaultLibraryWithBundle:[NSBundle bundleForClass:self.class] error:&error];
    NSAssert(defaultLibrary, @"Failed to create shader library: %@", error);
    id<MTLFunction> vertexFunction = [defaultLibrary newFunctionWithName:@"vertexShader"];
    id<MTLFunction> fragmentFunction = [defaultLibrary newFunctionWithName:@"fragmentShader"];
    MTLRenderPipelineDescriptor* pipelineStateDescriptor = [MTLRenderPipelineDescriptor new];
    pipelineStateDescriptor.label = @"Triangle Pipeline";
    pipelineStateDescriptor.vertexFunction = vertexFunction;
    pipelineStateDescriptor.fragmentFunction = fragmentFunction;
    pipelineStateDescriptor.colorAttachments[0].pixelFormat = merc::vst::toMTLPixelFormat(arrangement.textureBus.type);
    pipelineStateDescriptor.rasterSampleCount = 4;
    pipelineState = [graphics.device newRenderPipelineStateWithDescriptor:pipelineStateDescriptor
                                                                    error:&error];
    NSAssert(pipelineState, @"Failed to create pipeline state: %@", error);

    auto descriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:merc::vst::toMTLPixelFormat(arrangement.textureBus.type)
                                                                         width:arrangement.textureBus.width
                                                                        height:arrangement.textureBus.height
                                                                     mipmapped:false];
    descriptor.textureType = MTLTextureType2DMultisample;
    descriptor.sampleCount = pipelineStateDescriptor.rasterSampleCount;
    descriptor.usage = MTLTextureUsageRenderTarget;
    multisampleTexture = [graphics.device newTextureWithDescriptor:descriptor];
}

- (void)process:(const merc::vst::Graphics&)graphics
           data:(merc::vst::VideoData&)data
{
    id<MTLCommandBuffer> commandBuffer = [graphics.commandQueue commandBuffer];
    commandBuffer.label = @"Triangle";
    auto renderPassDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];
    auto colorAttachmentDescriptor = [MTLRenderPassColorAttachmentDescriptor new];
    colorAttachmentDescriptor.texture = multisampleTexture;
    colorAttachmentDescriptor.resolveTexture = data.outputs[0].texture->mtlTexture;
    colorAttachmentDescriptor.loadAction = MTLLoadActionClear;
    colorAttachmentDescriptor.storeAction = MTLStoreActionMultisampleResolve;
    [renderPassDescriptor.colorAttachments setObject:colorAttachmentDescriptor
                                  atIndexedSubscript:0];
    renderEncoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
    renderEncoder.label = @"Triangle";
    [renderEncoder setRenderPipelineState:pipelineState];

    voicePool->runVoices(*data.inputEvents, self);

    [renderEncoder endEncoding];
    [commandBuffer commit];
}

@synthesize renderEncoder;
@end

void TriangleVoice::start(MercTriangle* context, const Steinberg::Vst::Event& event)
{

}

void TriangleVoice::play(MercTriangle* context)
{
  static const Vertex triangleVertices[] =
  {
      { {  0.25,  -0.25 }, { 0.1,.1,.1, 1 } },
      { { -0.25,  -0.25 }, { 0,  .8,.8, 1 } },
      { {     0,   0.25 }, { 0,   1, 1, 1 } },
  };
  [context.renderEncoder setVertexBytes:triangleVertices
                                 length:sizeof(triangleVertices)
                                atIndex:0];
  [context.renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle
                            vertexStart:0
                            vertexCount:3];
}

void TriangleVoice::stop(MercTriangle* context, const Steinberg::Vst::Event& event)
{

}

struct Triangle : merc::vst::VideoEffect
{
    static const char* getCategory() { return kVstVideoEffectClass; }
    static const char* getSubCategory() { return merc::vst::video_sub_category::pixels; }
    static const char* getName() { return "triangle"; }
    Triangle()
    {
        addEventInput(u"EventInput");
        addVideoOutput(u"Output");

        mercTriangle = [[MercTriangle alloc] init];
    }
    Steinberg::tresult PLUGIN_API setBusArrangements(const merc::vst::Graphics& graphics,
                                                     merc::vst::VideoBusArrangement* inputs, Steinberg::int32 numIns,
                                                     merc::vst::VideoBusArrangement* outputs, Steinberg::int32 numOuts) override
    {
        [mercTriangle setArrangement:outputs[0]
                            graphics:graphics];
        return Steinberg::kResultOk;
    }
    Steinberg::tresult PLUGIN_API process(const merc::vst::Graphics& graphics, merc::vst::VideoData& videoData) override
    {
        [mercTriangle process:graphics
                         data:videoData];
        return Steinberg::kResultOk;
    }

    MercTriangle* mercTriangle;
};

template struct merc::vst::Module<Triangle>;
