#include <merc/module.h>
#include <merc/deltick-faust.h>

struct Deltick : Steinberg::Vst::AudioEffect
{
    static const char* getCategory() { return kVstAudioEffectClass; }
    static const char* getSubCategory() { return Steinberg::Vst::PlugType::kFx; }
    static const char* getName() { return "deltick"; }
    Deltick()
    {
        addAudioInput(u"Input", Steinberg::Vst::SpeakerArr::kMono);
        addAudioOutput(u"Output", Steinberg::Vst::SpeakerArr::kMono);
    }
    Steinberg::tresult PLUGIN_API setActive(Steinberg::TBool state) override
    {
        if (state) faustDeltick.init(processSetup.sampleRate, processSetup.maxSamplesPerBlock);
        return Steinberg::kResultOk;
    }
    Steinberg::tresult PLUGIN_API process(Steinberg::Vst::ProcessData& data) override
    {
        faustDeltick.process(data.numSamples,
                             data.inputs[0].channelBuffers32,
                             data.outputs[0].channelBuffers32);
        return Steinberg::kResultOk;
    }
    Steinberg::uint32 PLUGIN_API getLatencySamples() override
    {
        return 1000;
    }

    merc::faust::deltick faustDeltick;
};

template struct merc::Module<Deltick>;
