#pragma once

#include <public.sdk/source/vst/vstcomponent.h>
#include "illumia/ivstvideoprocessor.h"

namespace illumia
{
	class VideoBus : public Steinberg::Vst::Bus
	{
	public:
		VideoBus(const Steinberg::Vst::TChar* name, Steinberg::Vst::BusType busType, Steinberg::int32 flags);
		OBJ_METHODS(VideoBus, Steinberg::Vst::Bus);
    };

	class VideoEffect : public Steinberg::Vst::Component, public IVideoProcessor
	{
	public:
		Steinberg::tresult PLUGIN_API initialize(Steinberg::FUnknown* context) override;
        Steinberg::tresult PLUGIN_API activateBus(Steinberg::Vst::MediaType type, Steinberg::Vst::BusDirection dir, Steinberg::int32 index, Steinberg::TBool state) override;
        Steinberg::tresult PLUGIN_API getBusArrangement(Steinberg::Vst::BusDirection dir, Steinberg::int32 index, VideoBusArrangement& bus) override;

		OBJ_METHODS(VideoEffect, Steinberg::Vst::Component)
			DEFINE_INTERFACES
			DEF_INTERFACE(IVideoProcessor)
			END_DEFINE_INTERFACES(Steinberg::Vst::Component)
			REFCOUNT_METHODS(Steinberg::Vst::Component)
	protected:
		std::vector<Steinberg::OPtr<VideoBus>> videoInputs, videoOutputs;

		void addVideoInput(const Steinberg::Vst::TChar* name, Steinberg::Vst::BusType busType =  Steinberg::Vst::kMain,
						   Steinberg::int32 flags = Steinberg::Vst::BusInfo::kDefaultActive);
		void addVideoOutput(const Steinberg::Vst::TChar* name, Steinberg::Vst::BusType busType =  Steinberg::Vst::kMain,
							Steinberg::int32 flags = Steinberg::Vst::BusInfo::kDefaultActive);
		void addEventInput(const Steinberg::Vst::TChar* name, Steinberg::int32 channels = 16, Steinberg::Vst::BusType busType = Steinberg::Vst::kMain,
						   Steinberg::int32 flags = Steinberg::Vst::BusInfo::kDefaultActive);
		void addEventOutput(const Steinberg::Vst::TChar* name, Steinberg::int32 channels = 16, Steinberg::Vst::BusType busType = Steinberg::Vst::kMain,
							Steinberg::int32 flags = Steinberg::Vst::BusInfo::kDefaultActive);
	};
}
