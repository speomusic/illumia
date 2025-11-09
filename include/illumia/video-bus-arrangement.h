#pragma once

#include <pluginterfaces/vst/vsttypes.h>

namespace illumia
{
    using TextureBusType = unsigned;
    const TextureBusType kBGRA8Unorm{ 0 };

    using VideoBusType = Steinberg::uint32;
    const VideoBusType kTextureBus{ 0 };

    struct TextureBusArrangement
    {
        Steinberg::uint32 width{};
        Steinberg::uint32 height{};
        TextureBusType type{ kBGRA8Unorm };
    };

    struct VideoBusArrangement
    {
        VideoBusType type;
        union
        {
            TextureBusArrangement textureBus;
        };
    };
}
