#pragma once

#include <pluginterfaces/vst/ivstevents.h>
#include <array>
#include <span>
#include <unordered_map>
#include <string>
#include <string_view>
#include <cmath>
#include <algorithm>

namespace illumia::faust
{
    struct NoteCV
    {
        NoteCV(int busIndex, int channelMask);
        void set(Steinberg::Vst::IEventList* eventList);
        void get(Steinberg::Vst::IEventList* eventList, int channel);
        std::vector<float> gain, note;
    private:
        void add(const Steinberg::Vst::Event& e);
        void add(int sampleOffset, const Steinberg::Vst::NoteOnEvent& e);
        void add(int sampleOffset, const Steinberg::Vst::NoteOffEvent& e);
        float getPreviousGain(int index) const;
        int busIndex{}, channelMask{};
        float previousBufferGain{};
    };

    struct FaustWrapperBase
    {
        void declare(const char*, const char*) {}
        void openVerticalBox(const char*) {}
        void closeBox() {}
        void addVerticalSlider(const char* name, float* ptr, float init, float min, float max, float step);

        void setValue(std::string_view key, float valueZeroOne);
        std::vector<NoteCV> noteCVs;
    protected:
        void init(int maxBatchSize, std::vector<NoteCV> noteCVs);
        struct Value
        {
            float* ptr;
            float min, range, step;
        };
        std::unordered_map<std::string, Value> values;
        std::vector<float> emptyInput, ignoredOutput;
    };
    using Meta = FaustWrapperBase;
    using UI = FaustWrapperBase;
    struct dsp {};
    
    template<typename T>
    struct FaustWrapper : FaustWrapperBase
    {
        void init(int sampleRate, int maxBatchSize, std::vector<NoteCV> noteCVs = {})
        {
            FaustWrapperBase::init(maxBatchSize, std::move(noteCVs));
            generatedDsp.init(sampleRate);
            generatedDsp.buildUserInterface(this);
        }
        void process(int numFrames,
                     float** inputs,
                     float** outputs)
        {
            float* sanitizedInputs[std::max(T::getStaticNumInputs(), 1)];
            for (int i{ 0 }; i < T::getStaticNumInputs(); ++i)
                sanitizedInputs[i] = inputs[i] ? inputs[i] : emptyInput.data();
            float* sanitizedOutputs[std::max(T::getStaticNumOutputs(), 1)];
            for (int i{ 0 }; i < T::getStaticNumOutputs(); ++i)
                sanitizedOutputs[i] = outputs[i] ? outputs[i] : ignoredOutput.data();
            generatedDsp.compute(numFrames, sanitizedInputs, sanitizedOutputs);
        }
    private:
        T generatedDsp;
    };
}
