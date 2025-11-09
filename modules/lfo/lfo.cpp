#include <merc/module.h>
#include <merc/lfo-faust.h>

struct Lfo : Steinberg::Vst::AudioEffect
{
    static const char* getCategory() { return kVstAudioEffectClass; }
    static const char* getSubCategory() { return Steinberg::Vst::PlugType::kFx; }
    static const char* getName() { return "lfo"; }
    Lfo()
    {
        addAudioOutput(u"Output", Steinberg::Vst::SpeakerArr::kMono, Steinberg::Vst::kMain,
                       Steinberg::Vst::BusInfo::kDefaultActive | Steinberg::Vst::BusInfo::kIsControlVoltage);
    }
    Steinberg::tresult PLUGIN_API setActive(Steinberg::TBool state) override
    {
        if (state)
        {
            faustLfo.init(processSetup.sampleRate, processSetup.maxSamplesPerBlock);
            faustLfo.setValue("freq", 1.0f);
        }
        return Steinberg::kResultOk;
    }
    Steinberg::tresult PLUGIN_API process(Steinberg::Vst::ProcessData& data) override
    {
        faustLfo.process(data.numSamples, nullptr, data.outputs[0].channelBuffers32);
        return Steinberg::kResultOk;
    }

    merc::faust::lfo faustLfo;
};

template struct merc::Module<Lfo>;
