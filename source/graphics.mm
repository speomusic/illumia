#import "illumia/graphics.h"
#import "illumia/factory.h"

namespace illumia {
Factory::Factory() : graphics{std::make_unique<Graphics>()} {}

Factory::~Factory() = default;

Graphics::Graphics() {
  device = MTLCreateSystemDefaultDevice();
  commandQueue = [device newCommandQueue];
}
} // namespace illumia
