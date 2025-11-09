#include <merc/module.h>
#include <merc/zaval-faust.h>

struct Zaval : Steinberg::Vst::AudioEffect
{
    static const char* getCategory() { return kVstAudioEffectClass; }
    static const char* getSubCategory() { return Steinberg::Vst::PlugType::kInstrument; }
    static const char* getName() { return "zaval"; }
    Zaval()
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
//            faustZaval.init(processSetup.sampleRate, processSetup.maxSamplesPerBlock, std::move(noteCVs));
            faustZaval.init(processSetup.sampleRate, processSetup.maxSamplesPerBlock);
        }
        return Steinberg::kResultOk;
    }
    Steinberg::tresult PLUGIN_API setProcessing(Steinberg::TBool state) override
    {
        // Might be called on the audio thread! Not the right place for faustZaval.init().
        return Steinberg::kResultOk;
    }
    Steinberg::tresult PLUGIN_API process(Steinberg::Vst::ProcessData& data) override
    {
//        faustZaval.noteCVs.at(0).set(data.inputEvents);
//        float* inputs[]
//        {
//            faustZaval.noteCVs.at(0).gain.data(),
//            faustZaval.noteCVs.at(0).note.data(),
//            data.inputs[0].channelBuffers32[0]
//        };
        faustZaval.process(data.numSamples,
                          nullptr,//inputs,
                          data.outputs[0].channelBuffers32);
        return Steinberg::kResultOk;
    }

    merc::faust::zaval faustZaval;
};

template struct merc::Module<Zaval>;
