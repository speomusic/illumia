#import <Metal/Metal.h>
#import <MetalPerformanceShaders/MetalPerformanceShaders.h>
#import <merc/graphics.h>
#import <merc/module.h>
#import "metal-types.h"

@interface MercBloom : NSObject
- (id)initWithDevice:(id<MTLDevice>)device
       inArrangement:(merc::VideoBusArrangement)inArrangement
      outArrangement:(merc::VideoBusArrangement)outArrangement;
- (void)process:(const merc::Graphics&)graphics
           data:(merc::VideoData&)data;
@end

@implementation MercBloom
{
    id<MTLComputePipelineState> extractPipeline;
    id<MTLComputePipelineState> overlayPipeline;
    MPSImageGaussianBlur* blur;
    id<MTLTexture> overlayTexture;
    id<MTLTexture> blurredTexture;
    BloomParams params;
}

- (id)initWithDevice:(id<MTLDevice>)device
       inArrangement:(merc::VideoBusArrangement)inArrangement
      outArrangement:(merc::VideoBusArrangement)outArrangement
{
    self = [super init];
    if (!self) return nil;

    NSError *error;
    id<MTLLibrary> defaultLibrary = [device newDefaultLibraryWithBundle:[NSBundle bundleForClass:self.class] error:&error];
    NSAssert(defaultLibrary, @"Failed to create shader library: %@", error);
    id<MTLFunction> extractFunction = [defaultLibrary newFunctionWithName:@"bloomExtract"];
    id<MTLFunction> overlayFunction = [defaultLibrary newFunctionWithName:@"bloomOverlay"];
    extractPipeline = [device newComputePipelineStateWithFunction:extractFunction
                                                            error:&error];
    NSAssert(extractPipeline, @"Failed to create extract pipeline state: %@", error);
    overlayPipeline = [device newComputePipelineStateWithFunction:overlayFunction
                                                            error:&error];
    NSAssert(extractPipeline, @"Failed to create overlay pipeline state: %@", error);

    blur = [[MPSImageGaussianBlur alloc] initWithDevice:device sigma:params.sigma];
    auto descriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatBGRA8Unorm
                                                                         width:inArrangement.textureBus.width
                                                                        height:inArrangement.textureBus.height
                                                                     mipmapped:false];
    descriptor.usage = MTLTextureUsageShaderRead | MTLTextureUsageShaderWrite;
    overlayTexture = [device newTextureWithDescriptor:descriptor];
    blurredTexture = [device newTextureWithDescriptor:descriptor];
    return self;
}

- (void)process:(const merc::Graphics&)graphics
           data:(merc::VideoData&)data
{
    id<MTLCommandBuffer> commandBuffer = [graphics.commandQueue commandBuffer];
    commandBuffer.label = @"Bloom";
    const auto gridSize = MTLSizeMake(data.inputs[0].texture->mtlTexture.width,
                                      data.inputs[0].texture->mtlTexture.height,
                                      1);
    const auto threadGroupSizeX = std::min(data.inputs[0].texture->mtlTexture.width, extractPipeline.threadExecutionWidth);
    const auto threadGroupSize = MTLSizeMake(threadGroupSizeX,
                                             std::min(data.inputs[0].texture->mtlTexture.height, extractPipeline.maxTotalThreadsPerThreadgroup) / threadGroupSizeX,
                                             1);
    {
        auto computeEncoder = [commandBuffer computeCommandEncoder];
        [computeEncoder setComputePipelineState:extractPipeline];
        [computeEncoder setTexture:data.inputs[0].texture->mtlTexture atIndex:0];
        [computeEncoder setTexture:overlayTexture atIndex:1];
        [computeEncoder setBytes:&params length:sizeof(BloomParams) atIndex:0];
        [computeEncoder dispatchThreads:gridSize threadsPerThreadgroup:threadGroupSize];
        [computeEncoder endEncoding];
    }
    [blur encodeToCommandBuffer:commandBuffer
                  sourceTexture:overlayTexture
             destinationTexture:blurredTexture];
    {
        auto computeEncoder = [commandBuffer computeCommandEncoder];
        [computeEncoder setComputePipelineState:overlayPipeline];
        [computeEncoder setTexture:data.inputs[0].texture->mtlTexture atIndex:0];
        [computeEncoder setTexture:blurredTexture atIndex:1];
        [computeEncoder setTexture:data.outputs[0].texture->mtlTexture atIndex:2];
        [computeEncoder setBytes:&params length:sizeof(BloomParams) atIndex:0];
        [computeEncoder dispatchThreads:gridSize threadsPerThreadgroup:threadGroupSize];
        [computeEncoder endEncoding];
    }
    [commandBuffer commit];
}
@end

struct Bloom : merc::VideoEffect
{
    static const char* getCategory() { return kVstVideoEffectClass; }
    static const char* getSubCategory() { return merc::video_sub_category::pixels; }
    static const char* getName() { return "bloom"; }
    Bloom()
    {
        addVideoInput(u"Input");
        addVideoOutput(u"Output");
    }
    Steinberg::tresult PLUGIN_API setBusArrangements(const merc::Graphics& graphics,
                                                     merc::VideoBusArrangement* inputs, Steinberg::int32 numIns,
                                                     merc::VideoBusArrangement* outputs, Steinberg::int32 numOuts) override
    {
        if (numIns < 1 || numOuts < 1) return Steinberg::kNotImplemented;
        mercBloom = [[MercBloom alloc] initWithDevice:graphics.device
                                        inArrangement:inputs[0]
                                      outArrangement:outputs[0]];
        return Steinberg::kResultOk;
    }
    Steinberg::tresult PLUGIN_API process(const merc::Graphics& graphics, merc::VideoData& videoData) override
    {
        [mercBloom process:graphics
                      data:videoData];
        return Steinberg::kResultOk;
    }

    MercBloom* mercBloom;
};

template struct merc::Module<Bloom>;
