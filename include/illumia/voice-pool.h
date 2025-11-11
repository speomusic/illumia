#pragma once

#include <illumia/vst-utils.h>
#include <pluginterfaces/vst/ivstevents.h>
#include <vector>

namespace illumia {
template <typename VoiceImpl> struct Voice {
  bool playing{false};
  Steinberg::Vst::NoteOnEvent noteOnEvent{};
  VoiceImpl impl;

  Voice &asPlaying(const Steinberg::Vst::NoteOnEvent &noe) {
    playing = true;
    noteOnEvent = noe;
    return *this;
  }
};

template <typename VoiceImpl> struct VoicePool {
  VoicePool(size_t numVoices) : voices(numVoices) {}
  void runVoices(Steinberg::Vst::IEventList &eventList,
                 typename VoiceImpl::Context *context) {
    for (int eventIndex{0}; eventIndex < eventList.getEventCount();
         ++eventIndex) {
      Steinberg::Vst::Event event;
      CHECK_TRESULT(eventList.getEvent(eventIndex, event));
      switch (event.type) {
      case Steinberg::Vst::Event::kNoteOnEvent:
        getNonPlayingVoice(context)
            .asPlaying(event.noteOn)
            .impl.start(context, event);
        break;
      case Steinberg::Vst::Event::kNoteOffEvent:
        stopVoice(context, event);
        break;
      default:
        break;
      }
    }
    for (auto &voice : voices) {
      if (!voice.playing)
        continue;
      voice.impl.play(context);
    }
  }

private:
  std::vector<Voice<VoiceImpl>> voices;

  Voice<VoiceImpl> &getNonPlayingVoice(typename VoiceImpl::Context *context) {
    for (auto &voice : voices)
      if (!voice.playing)
        return voice;
    stopVoice(voices.front(), context);
    return voices.front();
  }

  void stopVoice(typename VoiceImpl::Context *context,
                 const Steinberg::Vst::Event &event) {
    if (auto *voice{tryGetVoiceToStop(event.noteOff)}; voice)
      stopVoice(*voice, context, event);
  }

  void stopVoice(Voice<VoiceImpl> &voice,
                 typename VoiceImpl::Context *context) {
    Steinberg::Vst::NoteOffEvent event{.channel = voice.noteOnEvent.channel,
                                       .pitch = voice.noteOnEvent.pitch,
                                       .velocity = 0.0f,
                                       .noteId = voice.noteOnEvent.noteId,
                                       .tuning = voice.noteOnEvent.tuning};
    voice.playing = false;
    voice.impl.stop(context, {.type = Steinberg::Vst::Event::kNoteOffEvent,
                              .noteOff = event});
  }

  void stopVoice(Voice<VoiceImpl> &voice, typename VoiceImpl::Context *context,
                 const Steinberg::Vst::Event &event) {
    voice.playing = false;
    voice.impl.stop(context, event);
  }

  Voice<VoiceImpl> *
  tryGetVoiceToStop(const Steinberg::Vst::NoteOffEvent &event) {
    for (auto &voice : voices) {
      if (voice.noteOnEvent.channel == event.channel &&
          voice.noteOnEvent.pitch == event.pitch &&
          voice.noteOnEvent.noteId == event.noteId)
        return &voice;
    }
    return nullptr;
  }
};
} // namespace illumia
