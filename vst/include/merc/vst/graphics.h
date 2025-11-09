#pragma once

#include <stdexcept>
#include "merc/vst/ivstvideoprocessor.h"

#if defined(_WIN32)
#include <d3d12.h>
#elif defined(__APPLE__) && defined(__OBJC__)
#import <Metal/Metal.h>
#import <MetalKit/MetalKit.h>
#endif

namespace merc::vst
{
    struct Graphics
    {
        Graphics();
#if defined(_WIN32)

#elif defined(__APPLE__) && defined(__OBJC__)
        id<MTLDevice> device;
        id<MTLCommandQueue> commandQueue;
#endif
    };

#if defined(_WIN32)

#elif defined(__APPLE__) && defined(__OBJC__)
    inline MTLPixelFormat toMTLPixelFormat(TextureBusType TextureBusType)
    {
        switch (TextureBusType)
        {
        case kBGRA8Unorm: return MTLPixelFormatBGRA8Unorm;
        default: throw std::runtime_error{ "Unsupported TextureBusType" };
        }
    }
#endif

#if defined(_WIN32)

#elif defined(__APPLE__) && defined(__OBJC__)
    struct Texture
    {
        Texture(const TextureBusArrangement& arrangement, id<MTLDevice> device)
        {
            auto descriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:toMTLPixelFormat(arrangement.type)
                                                                                 width:arrangement.width
                                                                                height:arrangement.height
                                                                             mipmapped:false];
            descriptor.usage = MTLTextureUsageShaderRead | MTLTextureUsageShaderWrite | MTLTextureUsageRenderTarget;
            mtlTexture = [device newTextureWithDescriptor:descriptor];
        }
        Texture(const Texture& other)
        {
            auto descriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:other.mtlTexture.pixelFormat
                                                                                 width:other.mtlTexture.width
                                                                                height:other.mtlTexture.height
                                                                             mipmapped:false];
            descriptor.usage = other.mtlTexture.usage;
            mtlTexture = [other.mtlTexture.device newTextureWithDescriptor:descriptor];
        }
        id<MTLTexture> mtlTexture;
    };
#endif
}
