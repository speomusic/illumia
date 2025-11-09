#include "illumia/vstvideoeffect.h"

namespace illumia
{
    DEF_CLASS_IID(IVideoProcessor)

    VideoBus::VideoBus(const Steinberg::Vst::TChar* name, Steinberg::Vst::BusType busType, Steinberg::int32 flags)
        : Bus{ name, busType, flags }
    {}

    Steinberg::tresult VideoEffect::initialize(Steinberg::FUnknown* context)
    {
        if (auto result{ Component::initialize(context) }; result != Steinberg::kResultOk) return result;
        return Steinberg::kResultOk;
    };


    Steinberg::tresult VideoEffect::activateBus(Steinberg::Vst::MediaType type, Steinberg::Vst::BusDirection dir, Steinberg::int32 index, Steinberg::TBool state)
    {
        switch (type)
        {
        case kVideo:
        {
            auto busList{ dir == Steinberg::Vst::kInput ? &videoInputs : &videoOutputs };
            if (index >= busList->size()) return Steinberg::kInvalidArgument;
            busList->at(index)->setActive(state);
            return Steinberg::kResultTrue;
        }
        default:
            return Steinberg::Vst::Component::activateBus(type, dir, index, state);
        }
    }

    Steinberg::tresult VideoEffect::getBusArrangement(Steinberg::Vst::BusDirection dir, Steinberg::int32 index, VideoBusArrangement& bus)
    {
        return Steinberg::kNotImplemented;
    }

    void VideoEffect::addVideoInput(const Steinberg::Vst::TChar* name, Steinberg::Vst::BusType busType, Steinberg::int32 flags)
    {
        auto* newBus = new VideoBus(name, busType, flags);
        videoInputs.push_back(Steinberg::OPtr<VideoBus>(newBus));
    }

    void VideoEffect::addVideoOutput(const Steinberg::Vst::TChar* name, Steinberg::Vst::BusType busType, Steinberg::int32 flags)
    {
        auto* newBus = new VideoBus(name, busType, flags);
        videoOutputs.push_back(Steinberg::OPtr<VideoBus>(newBus));
    }

    void VideoEffect::addEventInput(const Steinberg::Vst::TChar* name, Steinberg::int32 channels, Steinberg::Vst::BusType busType, Steinberg::int32 flags)
    {
        auto* newBus = new Steinberg::Vst::EventBus(name, busType, flags, channels);
        eventInputs.push_back(Steinberg::OPtr<Steinberg::Vst::Bus>(newBus));
    }

    void VideoEffect::addEventOutput(const Steinberg::Vst::TChar* name, Steinberg::int32 channels, Steinberg::Vst::BusType busType, Steinberg::int32 flags)
    {
        auto* newBus = new Steinberg::Vst::EventBus(name, busType, flags, channels);
        eventOutputs.push_back(Steinberg::OPtr<Steinberg::Vst::Bus>(newBus));
    }
}
