#pragma once

#include <filesystem>
#include "merc/structure/config.h"

namespace merc::global
{
    structure::Config& loadConfig(std::filesystem::path configPath);
    structure::Config& getConfig();
    const std::filesystem::path& getConfigPath();
    void saveConfig();

    static struct ConfigUser { ConfigUser(); ~ConfigUser(); } configUser{};
}
