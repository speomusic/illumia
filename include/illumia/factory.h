#pragma once

#include <memory>

namespace merc::vst
{
    struct Factory
    {
        Factory();
        ~Factory();
        std::unique_ptr<struct Graphics> graphics;
    };
}
