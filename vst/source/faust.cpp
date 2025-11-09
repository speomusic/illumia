#include "merc/vst/faust.h"
#include "merc/vst/vst-utils.h"

namespace merc::faust
{
    void FaustWrapperBase::addVerticalSlider(const char* name, float* ptr, float init, float min, float max, float step)
    {
        values.insert({ name, Value{ ptr, min, max - min, step } });
    }

    void FaustWrapperBase::setValue(std::string_view key, float valueZeroOne)
    {
        auto& value{ values.at(std::string{ key }) };
        const auto steps{ std::floorf(valueZeroOne * value.range / value.step) };
        *value.ptr = value.min + steps * value.step;
    }

    NoteCV::NoteCV(int busIndex, int channelMask)
        : busIndex{ busIndex }
        , channelMask{ channelMask }
    {}

    void NoteCV::set(Steinberg::Vst::IEventList* eventList)
    {
        std::fill(gain.begin(), gain.end(), gain.back());
        std::fill(note.begin(), note.end(), note.back());
        if (!eventList) return;
        for (int i{ 0 }; i < eventList->getEventCount(); ++i)
        {
            Steinberg::Vst::Event e;
            CHECK_TRESULT(eventList->getEvent(i, e));
            if (e.busIndex == busIndex) add(e);
        }
    }

    void NoteCV::get(Steinberg::Vst::IEventList* eventList, int channel)
    {
        if (!eventList) return;
        for (int i{ 0 }; i < gain.size(); ++i)
        {
            if (getPreviousGain(i) == 0.0f && gain[i] != 0.0f)
            {
                Steinberg::Vst::Event e{ busIndex, i, 0.0, 0, Steinberg::Vst::Event::kNoteOnEvent };
                e.noteOn = Steinberg::Vst::NoteOnEvent
                {
                    (Steinberg::int16)channel,
                    (Steinberg::int16)note[i],
                    0.0f, gain[i], -1, -1
                };
                CHECK_TRESULT(eventList->addEvent(e));
            }
            else if (getPreviousGain(i) != 0.0f && gain[i] == 0.0f)
            {
                Steinberg::Vst::Event e{ busIndex, i, 0.0, 0, Steinberg::Vst::Event::kNoteOffEvent };
                e.noteOff = Steinberg::Vst::NoteOffEvent
                {
                    (Steinberg::int16)channel,
                    (Steinberg::int16)note[i],
                    0.0f, -1, 0.0f
                };
                CHECK_TRESULT(eventList->addEvent(e));
            }
        }
        previousBufferGain = gain.back();
    }

    void NoteCV::add(const Steinberg::Vst::Event& e)
    {
        switch (e.type)
        {
        case Steinberg::Vst::Event::kNoteOnEvent:
            add(e.sampleOffset, e.noteOn);
            break;
        case Steinberg::Vst::Event::kNoteOffEvent:
            add(e.sampleOffset, e.noteOff);
            break;
        }
    }

    void NoteCV::add(int sampleOffset, const Steinberg::Vst::NoteOnEvent& e)
    {
        if (!(1 << e.channel & channelMask)) return;
        for (int i{ sampleOffset }; i < gain.size(); ++i)
        {
            gain[i] = e.velocity;
            note[i] = e.pitch;
        }
    }

    void NoteCV::add(int sampleOffset, const Steinberg::Vst::NoteOffEvent& e)
    {
        if (!(1 << e.channel & channelMask)) return;
        for (int i{ sampleOffset }; i < gain.size(); ++i)
        {
            gain[i] = 0.0f;
        }
    }

    float NoteCV::getPreviousGain(int index) const
    {
        if (index == 0) return previousBufferGain;
        return gain[index - 1];
    }

    void FaustWrapperBase::init(int maxBatchSize, std::vector<NoteCV> n)
    {
        emptyInput.resize(maxBatchSize);
        ignoredOutput.resize(maxBatchSize);
        noteCVs = std::move(n);
        for (auto& noteCV : noteCVs)
        {
            noteCV.gain.resize(maxBatchSize);
            noteCV.note.resize(maxBatchSize);
        }
    }
}
