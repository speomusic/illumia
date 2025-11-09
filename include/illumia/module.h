#pragma once

#include "merc/vst/module-instance.h"
#include "merc/vst/vstvideoeffect.h"
#include <public.sdk/source/vst/vstaudioeffect.h>
#include <span>
#include <utility>
#include <array>
#include <concepts>
#include <memory>

namespace merc::vst
{
    struct SampleRate
    {
        SampleRate(int v) : value{ v } {}
        operator int() const { return value; }
        int value;
    };

    template<typename Object>
    concept DerivedFromEffect = std::derived_from<Object, Steinberg::Vst::AudioEffect>
                             || std::derived_from<Object, merc::vst::VideoEffect>;

    template<typename Object>
    concept ValidObject = DerivedFromEffect<Object> && requires()
    {
        { Object::getCategory() } -> std::convertible_to<const char*>;
        { Object::getSubCategory() } -> std::convertible_to<const char*>;
        { Object::getName() } -> std::convertible_to<const char*>;
    };

    template<ValidObject... Objects>
    struct Module final : Steinberg::IPluginFactory2
    {
        static_assert(sizeof...(Objects) <= MERC_GENERATED_GUID_COUNT,
                      "You need more GUIDS!"
                      "Increase the GENERATED_GUID_COUNT option when adding this module in CMake.");

        std::array<Steinberg::TUID, sizeof...(Objects)> classGuids;

        Module()
        {
            moduleInstance = this;
            char mercGeneratedGuids[][33]{ MERC_GENERATED_GUIDS };
            for (int i { 0 }; i < sizeof...(Objects); ++i)
            {
                Steinberg::FUID fuid;
                fuid.fromString(mercGeneratedGuids[i]);
                fuid.toTUID(classGuids[i]);
            }
        }

        inline static std::unique_ptr<Module> instance{ std::make_unique<Module>() };

        Steinberg::tresult PLUGIN_API getFactoryInfo(Steinberg::PFactoryInfo* info) override
        {
            *info = Steinberg::PFactoryInfo
            {
                MERC_MODULE_VENDOR,
                MERC_MODULE_URL,
                MERC_MODULE_EMAIL,
                Steinberg::PFactoryInfo::kUnicode
            };
            return Steinberg::kResultOk;
        }
        Steinberg::tresult PLUGIN_API countClasses() override
        {
            return sizeof...(Objects);
        }
        Steinberg::tresult PLUGIN_API getClassInfo(int32_t index, Steinberg::PClassInfo* info) override
        {
            return getClassInfo(index, info, std::make_index_sequence<sizeof...(Objects)>{});
        }
        template<size_t... Is>
        Steinberg::tresult getClassInfo(int32_t index, Steinberg::PClassInfo* info, std::index_sequence<Is...>)
        {
            return (getClassInfo<Is, Objects>(index, info) || ...) ? Steinberg::kResultOk : Steinberg::kInvalidArgument;
        }
        template<size_t Index, typename Object>
        bool getClassInfo(int32_t index, Steinberg::PClassInfo* info)
        {
            if (Index != index) return false;
            *info = Steinberg::PClassInfo
            {
                classGuids[Index],
                Steinberg::PClassInfo::kManyInstances,
                Object::getCategory(),
                Object::getName()
            };
            return true;
        }
        Steinberg::tresult PLUGIN_API getClassInfo2(int32_t index, Steinberg::PClassInfo2* info) override
        {
            return getClassInfo2(index, info, std::make_index_sequence<sizeof...(Objects)>{});
        }
        template<size_t... Is>
        Steinberg::tresult getClassInfo2(int32_t index, Steinberg::PClassInfo2* info, std::index_sequence<Is...>)
        {
            return (getClassInfo2<Is, Objects>(index, info) || ...) ? Steinberg::kResultOk : Steinberg::kInvalidArgument;
        }
        template<size_t Index, typename Object>
        bool getClassInfo2(int32_t index, Steinberg::PClassInfo2* info)
        {
            if (Index != index) return false;
            *info = Steinberg::PClassInfo2
            {
                classGuids[Index],
                Steinberg::PClassInfo::kManyInstances,
                Object::getCategory(),
                Object::getName(),
                0,
                Object::getSubCategory(),
                MERC_MODULE_VENDOR,
                "1.0.0",
                kVstVersionString
            };
            return true;
        }
        Steinberg::tresult PLUGIN_API createInstance(const char* cid, const char* iid, void** obj) override
        {
            return createInstance(cid, iid, obj, std::make_index_sequence<sizeof...(Objects)>{});
        }
        template<size_t... Is>
        Steinberg::tresult createInstance(const char* cid, const char* iid, void** obj, std::index_sequence<Is...>)
        {
            Steinberg::tresult result{ Steinberg::kNoInterface };
            if ((createInstance<Is, Objects>(cid, iid, obj, result) || ...)) return result;
            *obj = nullptr;
            return result;
        }
        template<size_t Index, typename Object>
        bool createInstance(const char* cid, const char* iid, void** obj, int32_t& result)
        {
            if (!Steinberg::FUnknownPrivate::iidEqual(classGuids[Index], cid)) return false;
            auto instance{ Steinberg::owned(new Object()) };
            result = instance->queryInterface(iid, obj);
            return true;
        }
        Steinberg::tresult PLUGIN_API queryInterface(const char iid[16], void** obj) override
        {
            DEF_INTERFACE(Steinberg::IPluginFactory)
            DEF_INTERFACE(Steinberg::IPluginFactory2)
            *obj = nullptr;
            return Steinberg::kNoInterface;
        }
        uint32_t PLUGIN_API addRef() override
        {
            return 1;
        }
        uint32_t PLUGIN_API release() override
        {
            return 1;
        }
    };
}
