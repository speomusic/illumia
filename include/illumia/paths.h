#pragma once

#include <filesystem>

namespace merc::global
{
    std::filesystem::path getVstDefaultSystemDir();
    std::filesystem::path getVstDefaultUserDir();
    std::filesystem::path getModuleDir();
    std::filesystem::path getModuleResourceDir();
}
