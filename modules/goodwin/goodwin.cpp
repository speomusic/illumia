#include <merc/module.h>
#include <merc/goodwin-faust.h>

struct Goodwin : Steinberg::Vst::AudioEffect
{
    static const char* getCategory() { return kVstAudioEffectClass; }
    static const char* getSubCategory() { return Steinberg::Vst::PlugType::kInstrument; }
    static const char* getName() { return "goodwin"; }
    Goodwin()
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
//            faustGoodwin.init(processSetup.sampleRate, processSetup.maxSamplesPerBlock, std::move(noteCVs));
            faustGoodwin.init(processSetup.sampleRate, processSetup.maxSamplesPerBlock);
        }
        return Steinberg::kResultOk;
    }
    Steinberg::tresult PLUGIN_API setProcessing(Steinberg::TBool state) override
    {
        // Might be called on the audio thread! Not the right place for faustGoodwin.init().
        return Steinberg::kResultOk;
    }
    Steinberg::tresult PLUGIN_API process(Steinberg::Vst::ProcessData& data) override
    {
//        faustGoodwin.noteCVs.at(0).set(data.inputEvents);
//        float* inputs[]
//        {
//            faustGoodwin.noteCVs.at(0).gain.data(),
//            faustGoodwin.noteCVs.at(0).note.data(),
//            data.inputs[0].channelBuffers32[0]
//        };
        faustGoodwin.process(data.numSamples,
                          nullptr,//inputs,
                          data.outputs[0].channelBuffers32);
        return Steinberg::kResultOk;
    }

    merc::faust::goodwin faustGoodwin;
};

template struct merc::Module<Goodwin>;
