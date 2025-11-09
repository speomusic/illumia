#include <merc/module.h>
#include <merc/tick-faust.h>

struct Tick : Steinberg::Vst::AudioEffect
{
    static const char* getCategory() { return kVstAudioEffectClass; }
    static const char* getSubCategory() { return Steinberg::Vst::PlugType::kFx; }
    static const char* getName() { return "tick"; }
    Tick()
    {
        addAudioOutput(u"Output", Steinberg::Vst::SpeakerArr::kMono);
    }
    Steinberg::tresult PLUGIN_API setActive(Steinberg::TBool state) override
    {
        if (state) faustTick.init(processSetup.sampleRate, processSetup.maxSamplesPerBlock);
        return Steinberg::kResultOk;
    }
    Steinberg::tresult PLUGIN_API process(Steinberg::Vst::ProcessData& data) override
    {
        faustTick.process(data.numSamples,
                             nullptr,
                             data.outputs[0].channelBuffers32);
        return Steinberg::kResultOk;
    }
    Steinberg::uint32 PLUGIN_API getLatencySamples() override
    {
        return 0;
    }

    merc::faust::tick faustTick;
};

template struct merc::Module<Tick>;
