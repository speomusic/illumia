#pragma once

#include <pluginterfaces/base/funknown.h>
#include <pluginterfaces/vst/ivstcomponent.h>
#include <pluginterfaces/vst/ivstevents.h>
#include "illumia/video-bus-arrangement.h"

#ifndef kVstVideoEffectClass
#define kVstVideoEffectClass "Video Module Class"
#endif

namespace illumia
{
	namespace video_sub_category
	{
		constexpr const char* pixels{ "Pixels" };
	}

	enum IllumiaMediaTypes
	{
		kVideo = Steinberg::Vst::kNumMediaTypes,
		kNumMediaTypes
	};

    struct VideoBusData
    {
        union
        {
            struct Texture* texture;
        };
    };

    struct VideoData
    {
        Steinberg::int32 numInputs{};
        Steinberg::int32 numOutputs{};
        VideoBusData* inputs{};
        VideoBusData* outputs{};
        Steinberg::Vst::IEventList* inputEvents{};
        Steinberg::Vst::IEventList* outputEvents{};
    };

	// [extends IComponent]
	class IVideoProcessor : public Steinberg::FUnknown
	{
	public:
        virtual Steinberg::tresult PLUGIN_API setBusArrangements(const struct Graphics& graphics,
                                                                 VideoBusArrangement* inputs, Steinberg::int32 numIns,
                                                                 VideoBusArrangement* outputs, Steinberg::int32 numOuts) = 0;
        virtual Steinberg::tresult PLUGIN_API getBusArrangement(Steinberg::Vst::BusDirection dir, Steinberg::int32 index, VideoBusArrangement& bus) = 0;
        virtual Steinberg::tresult PLUGIN_API process(const struct Graphics& graphics, VideoData& videoData) = 0;

		static const Steinberg::FUID iid;
	};

	DECLARE_CLASS_IID(IVideoProcessor, 0xC729CEAF, 0x2DC44D9A, 0xB65D2209, 0xBBF0863E)
}
