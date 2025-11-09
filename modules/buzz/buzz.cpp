#include <merc/vst/module.h>
#include <merc/buzz-faust.h>

struct Buzz : Steinberg::Vst::AudioEffect
{
    static const char* getCategory() { return kVstAudioEffectClass; }
    static const char* getSubCategory() { return Steinberg::Vst::PlugType::kInstrument; }
    static const char* getName() { return "buzz"; }
    Buzz()
    {
//        addEventInput(u"Notes");
//        addAudioInput(u"Offset (Pitch)", Steinberg::Vst::SpeakerArr::kMono, Steinberg::Vst::kMain,
//                      Steinberg::Vst::BusInfo::kDefaultActive | Steinberg::Vst::BusInfo::kIsControlVoltage);
        addAudioOutput(u"Output", Steinberg::Vst::SpeakerArr::kStereo);
    }
    Steinberg::tresult PLUGIN_API setActive(Steinberg::TBool state) override
    {
        if (state)
        {
//            std::vector<merc::faust::NoteCV> noteCVs{ { 0, 0xFFFF } };
//            faustBuzz.init(processSetup.sampleRate, processSetup.maxSamplesPerBlock, std::move(noteCVs));
            faustBuzz.init(processSetup.sampleRate, processSetup.maxSamplesPerBlock);
        }
        return Steinberg::kResultOk;
    }
    Steinberg::tresult PLUGIN_API setProcessing(Steinberg::TBool state) override
    {
        // Might be called on the audio thread! Not the right place for faustBuzz.init().
        return Steinberg::kResultOk;
    }
    Steinberg::tresult PLUGIN_API process(Steinberg::Vst::ProcessData& data) override
    {
//        faustBuzz.noteCVs.at(0).set(data.inputEvents);
//        float* inputs[]
//        {
//            faustBuzz.noteCVs.at(0).gain.data(),
//            faustBuzz.noteCVs.at(0).note.data(),
//            data.inputs[0].channelBuffers32[0]
//        };
        faustBuzz.process(data.numSamples,
                          nullptr,//inputs,
                          data.outputs[0].channelBuffers32);
        return Steinberg::kResultOk;
    }

    merc::faust::buzz faustBuzz;
};

template struct merc::vst::Module<Buzz>;
