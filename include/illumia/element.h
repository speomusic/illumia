#pragma once

#include "illumia/gate.h"
#include "illumia/ivstvideoprocessor.h"
#include <public.sdk/source/vst/hosting/eventlist.h>

namespace illumia {
template <typename T> struct VstElementTraits;
template <> struct VstElementTraits<structure::Link> {
  using Resource = Samples;
  using Arrangement = Steinberg::Vst::SpeakerArrangement;
  static constexpr int defaultMediaType{Steinberg::Vst::kAudio};
  using Processor = Steinberg::Vst::IAudioProcessor;
};
template <> struct VstElementTraits<structure::Step> {
  using Resource = Pixels;
  using Arrangement = structure::VideoBusArrangement;
  static constexpr int defaultMediaType{vst::kVideo};
  using Processor = vst::IVideoProcessor;
};

struct TimedEvent {
  size_t sample;
  Steinberg::Vst::Event event;
};

template <typename T> struct ElementImplementation;

template <typename T> struct Element {
  Element(Steinberg::IPtr<Steinberg::Vst::IComponent> component);
  Element(const Element &) = delete;
  Element(Element &&) = delete;
  Element &operator=(const Element &) = delete;
  Element &operator=(Element &&) = delete;
  ~Element();
  void activate();
  void deactivate();
  void pushEvent(TimedEvent event);
  void run(const TimeKeeper<Element<T>> &timeKeeper);

  std::unique_ptr<Gate<TimedEvent>> eventGate;

private:
  bool canReceiveEvents() const;
  void pumpInputEvents(size_t startSampleIndex, size_t samplesPerRun);

  template <typename U> friend struct VstElementImplementation;

  Steinberg::IPtr<Steinberg::Vst::IComponent> component;
  Steinberg::IPtr<typename VstElementTraits<T>::Processor> processor;

  std::unique_ptr<Steinberg::Vst::EventList> inEvents, outEvents;

  ElementImplementation<T> impl;
};
} // namespace illumia
