#import "merc/vst/graphics.h"
#import "merc/vst/factory.h"

namespace merc::vst
{
    Factory::Factory()
        : graphics{ std::make_unique<Graphics>() }
    {}

    Factory::~Factory() = default;

    Graphics::Graphics()
    {
        device = MTLCreateSystemDefaultDevice();
        commandQueue = [device newCommandQueue];
    }
}
