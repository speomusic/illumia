#include <cstddef>
#include <filesystem>
#include "merc/global/config.h"
#include "generated-interface-persist.h"

namespace merc::global
{
    alignas(structure::Config) static std::byte configStorage[sizeof(structure::Config)];
    static std::filesystem::path loadedConfigPath{};
    static structure::UInt configUserCount{ 0 };

    structure::Config& loadConfig(std::filesystem::path configPath)
    {
        auto* config{ new (configStorage) structure::Config };
        if (std::filesystem::is_regular_file(configPath))
        {
            Steinberg::OPtr<Steinberg::IBStream> fileStream{ Steinberg::Vst::FileStream::open(configPath.string().c_str(), "rb") };
            Steinberg::IBStreamer in{ fileStream };
            if (interface::load<Steinberg::uint32>(in) != MERC_SERDE_FORMAT) throw "Serde format is not supported.";
            *config = interface::load<structure::Config>(in);
        }
        loadedConfigPath = std::move(configPath);
        return *config;
    }

    structure::Config& getConfig()
    {
        if (loadedConfigPath.empty())
            throw "Config used before loaded.";
        return *reinterpret_cast<structure::Config*>(configStorage);
    }

    const std::filesystem::path& getConfigPath()
    {
        return loadedConfigPath;
    }

    void saveConfig()
    {
        std::filesystem::create_directories(loadedConfigPath.parent_path());
        Steinberg::OPtr<Steinberg::IBStream> fileStream{ Steinberg::Vst::FileStream::open(loadedConfigPath.string().c_str(), "wb") };
        Steinberg::IBStreamer out{ fileStream };
        interface::save((Steinberg::uint32)MERC_SERDE_FORMAT, out);
        interface::save(getConfig(), out);
    }

    ConfigUser::ConfigUser()
    {
        ++configUserCount;
    }

    ConfigUser::~ConfigUser()
    {
        if (--configUserCount == 0 && !loadedConfigPath.empty())
            getConfig().~Config();
    }
}
