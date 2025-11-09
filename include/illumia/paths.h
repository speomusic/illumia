#pragma once

#include <filesystem>

namespace illumia {
std::filesystem::path getVstDefaultSystemDir();
std::filesystem::path getVstDefaultUserDir();
std::filesystem::path getModuleDir();
std::filesystem::path getModuleResourceDir();
} // namespace illumia
