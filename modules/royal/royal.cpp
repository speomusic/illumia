#include <merc/module.h>
#include <merc/royal-faust.h>

struct Royal : Steinberg::Vst::AudioEffect
{
    static const char* getCategory() { return kVstAudioEffectClass; }
    static const char* getSubCategory() { return Steinberg::Vst::PlugType::kFx; }
    static const char* getName() { return "royal"; }
    Royal()
    {
        addEventOutput(u"Output");
    }
    Steinberg::tresult PLUGIN_API setActive(Steinberg::TBool state) override
    {
        if (state)
        {
            std::vector<merc::faust::NoteCV> noteCVs{ { 0, 0xFFFF } };
            faustRoyal.init(processSetup.sampleRate, processSetup.maxSamplesPerBlock, std::move(noteCVs));
        }
        return Steinberg::kResultOk;
    }
    Steinberg::tresult PLUGIN_API process(Steinberg::Vst::ProcessData& data) override
    {
        float* outputs[]
        {
            faustRoyal.noteCVs.at(0).gain.data(),
            faustRoyal.noteCVs.at(0).note.data(),
        };
        faustRoyal.process(data.numSamples, nullptr, outputs);
        faustRoyal.noteCVs.at(0).get(data.outputEvents, 0);
        return Steinberg::kResultOk;
    }

    merc::faust::royal faustRoyal;
};

template struct merc::Module<Royal>;
