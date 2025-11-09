#include <merc/module.h>
#include <merc/trancegate-faust.h>

struct Trancegate : Steinberg::Vst::AudioEffect
{
    static const char* getCategory() { return kVstAudioEffectClass; }
    static const char* getSubCategory() { return Steinberg::Vst::PlugType::kFx; }
    static const char* getName() { return "trancegate"; }
    Trancegate()
    {
        addAudioInput(u"Input", Steinberg::Vst::SpeakerArr::kStereo);
        addAudioOutput(u"Output", Steinberg::Vst::SpeakerArr::kStereo);
    }
    Steinberg::tresult PLUGIN_API setActive(Steinberg::TBool state) override
    {
        if (state) faustTrancegate.init(processSetup.sampleRate, processSetup.maxSamplesPerBlock);
        return Steinberg::kResultOk;
    }
    Steinberg::tresult PLUGIN_API process(Steinberg::Vst::ProcessData& data) override
    {
        faustTrancegate.process(data.numSamples,
                                data.inputs[0].channelBuffers32,
                                data.outputs[0].channelBuffers32);
        return Steinberg::kResultOk;
    }

    merc::faust::trancegate faustTrancegate;
};

template struct merc::Module<Trancegate>;
