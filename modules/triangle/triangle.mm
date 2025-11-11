#include "metal-types.h"
#import <Metal/Metal.h>
#import <illumia/graphics.h>
#import <illumia/module.h>
#import <illumia/voice-pool.h>
#import <simd/simd.h>

@class IllumiaTriangle;

struct TriangleVoice {
  using Context = IllumiaTriangle;
  void start(IllumiaTriangle *context, const Steinberg::Vst::Event &event);
  void play(IllumiaTriangle *context);
  void stop(IllumiaTriangle *context, const Steinberg::Vst::Event &event);
};

@interface IllumiaTriangle : NSObject
- (id)init;
- (void)setArrangement:(illumia::VideoBusArrangement)arrangement
              graphics:(const illumia::Graphics &)graphics;
- (void)process:(const illumia::Graphics &)graphics
           data:(illumia::VideoData &)data;
@property id<MTLRenderCommandEncoder> renderEncoder;
@end

@implementation IllumiaTriangle {
  id<MTLRenderPipelineState> pipelineState;
  id<MTLTexture> multisampleTexture;
  std::unique_ptr<illumia::VoicePool<TriangleVoice>> voicePool;
}

- (id)init {
  self = [super init];
  if (!self)
    return nil;

  voicePool = std::make_unique<illumia::VoicePool<TriangleVoice>>(128);
  return self;
}

- (void)setArrangement:(illumia::VideoBusArrangement)arrangement
              graphics:(const illumia::Graphics &)graphics {
  NSError *error;
  id<MTLLibrary> defaultLibrary = [graphics.device
      newDefaultLibraryWithBundle:[NSBundle bundleForClass:self.class]
                            error:&error];
  NSAssert(defaultLibrary, @"Failed to create shader library: %@", error);
  id<MTLFunction> vertexFunction =
      [defaultLibrary newFunctionWithName:@"vertexShader"];
  id<MTLFunction> fragmentFunction =
      [defaultLibrary newFunctionWithName:@"fragmentShader"];
  MTLRenderPipelineDescriptor *pipelineStateDescriptor =
      [MTLRenderPipelineDescriptor new];
  pipelineStateDescriptor.label = @"Triangle Pipeline";
  pipelineStateDescriptor.vertexFunction = vertexFunction;
  pipelineStateDescriptor.fragmentFunction = fragmentFunction;
  pipelineStateDescriptor.colorAttachments[0].pixelFormat =
      illumia::toMTLPixelFormat(arrangement.textureBus.type);
  pipelineStateDescriptor.rasterSampleCount = 4;
  pipelineState = [graphics.device
      newRenderPipelineStateWithDescriptor:pipelineStateDescriptor
                                     error:&error];
  NSAssert(pipelineState, @"Failed to create pipeline state: %@", error);

  auto descriptor = [MTLTextureDescriptor
      texture2DDescriptorWithPixelFormat:illumia::toMTLPixelFormat(
                                             arrangement.textureBus.type)
                                   width:arrangement.textureBus.width
                                  height:arrangement.textureBus.height
                               mipmapped:false];
  descriptor.textureType = MTLTextureType2DMultisample;
  descriptor.sampleCount = pipelineStateDescriptor.rasterSampleCount;
  descriptor.usage = MTLTextureUsageRenderTarget;
  multisampleTexture = [graphics.device newTextureWithDescriptor:descriptor];
}

- (void)process:(const illumia::Graphics &)graphics
           data:(illumia::VideoData &)data {
  id<MTLCommandBuffer> commandBuffer = [graphics.commandQueue commandBuffer];
  commandBuffer.label = @"Triangle";
  auto renderPassDescriptor = [MTLRenderPassDescriptor renderPassDescriptor];
  auto colorAttachmentDescriptor = [MTLRenderPassColorAttachmentDescriptor new];
  colorAttachmentDescriptor.texture = multisampleTexture;
  colorAttachmentDescriptor.resolveTexture =
      data.outputs[0].texture->mtlTexture;
  colorAttachmentDescriptor.loadAction = MTLLoadActionClear;
  colorAttachmentDescriptor.storeAction = MTLStoreActionMultisampleResolve;
  [renderPassDescriptor.colorAttachments setObject:colorAttachmentDescriptor
                                atIndexedSubscript:0];
  renderEncoder =
      [commandBuffer renderCommandEncoderWithDescriptor:renderPassDescriptor];
  renderEncoder.label = @"Triangle";
  [renderEncoder setRenderPipelineState:pipelineState];

  voicePool->runVoices(*data.inputEvents, self);

  [renderEncoder endEncoding];
  [commandBuffer commit];
}

@synthesize renderEncoder;
@end

void TriangleVoice::start(IllumiaTriangle *context,
                          const Steinberg::Vst::Event &event) {}

void TriangleVoice::play(IllumiaTriangle *context) {
  static const Vertex triangleVertices[] = {
      {{0.25, -0.25}, {0.1, .1, .1, 1}},
      {{-0.25, -0.25}, {0, .8, .8, 1}},
      {{0, 0.25}, {0, 1, 1, 1}},
  };
  [context.renderEncoder setVertexBytes:triangleVertices
                                 length:sizeof(triangleVertices)
                                atIndex:0];
  [context.renderEncoder drawPrimitives:MTLPrimitiveTypeTriangle
                            vertexStart:0
                            vertexCount:3];
}

void TriangleVoice::stop(IllumiaTriangle *context,
                         const Steinberg::Vst::Event &event) {}

struct Triangle : illumia::VideoEffect {
  static const char *getCategory() { return kVstVideoEffectClass; }
  static const char *getSubCategory() {
    return illumia::video_sub_category::pixels;
  }
  static const char *getName() { return "triangle"; }
  Triangle() {
    addEventInput(u"EventInput");
    addVideoOutput(u"Output");

    illumiaTriangle = [[IllumiaTriangle alloc] init];
  }
  Steinberg::tresult PLUGIN_API setBusArrangements(
      const illumia::Graphics &graphics, illumia::VideoBusArrangement *inputs,
      Steinberg::int32 numIns, illumia::VideoBusArrangement *outputs,
      Steinberg::int32 numOuts) override {
    [illumiaTriangle setArrangement:outputs[0] graphics:graphics];
    return Steinberg::kResultOk;
  }
  Steinberg::tresult PLUGIN_API
  process(const illumia::Graphics &graphics,
          illumia::VideoData &videoData) override {
    [illumiaTriangle process:graphics data:videoData];
    return Steinberg::kResultOk;
  }

  IllumiaTriangle *illumiaTriangle;
};

template struct illumia::Module<Triangle>;
