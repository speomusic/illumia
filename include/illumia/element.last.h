#pragma once

#include "merc/av/resource.h"
#include "merc/av/revolver.h"
#include "merc/av/time-keeper.h"
#include "merc/structure/merc-structure.h"
#include <array>
#include <memory>
#include <pluginterfaces/vst/ivstaudioprocessor.h>
#include <variant>
#include <vector>

namespace illumia {
template <typename T> struct ElementTraits;

template <> struct ElementTraits<structure::Link> {
  using Resource = Samples;
  using Arrangement = Steinberg::Vst::SpeakerArrangement;
  static constexpr int defaultMediaType{Steinberg::Vst::kAudio};
};

template <> struct ElementTraits<structure::Step> {
  using Resource = Pixels;
  using Arrangement = structure::VideoBusArrangement;
  static constexpr int defaultMediaType{vst::kVideo};
};

struct TimedEvent {
  size_t sample;
  Steinberg::Vst::Event event;
};

template <typename T>
typename ElementTraits<T>::Resource
createResource(typename ElementTraits<T>::Arrangement arrangement,
               const struct Merc &merc);
template <typename T>
typename ElementTraits<T>::Arrangement getMainOutArrangement();

template <typename T, typename R> struct ElementImplementation;

template <typename T> struct Element {
  Element(structure::Runner runner, typename ElementTraits<T>::Routing routing,
          structure::Index index, struct Merc &merc);
  Element(const Element &) = delete;
  Element(Element &&) = delete;
  Element &operator=(Element &&) = delete;
  Element &operator=(const Element &) = delete;
  void changeRouting(typename ElementTraits<T>::Routing newRouting);
  void pushEvent(TimedEvent event);
  void run(TimeKeeper<Element<T>> &timeKeeper);
  void onRoutingAndMediaChanged();
  const typename ElementTraits<T>::Routing &getRouting() const {
    return routing;
  }
  const Media<T> &getMedia() const { return media; }
  bool hasMercInOrOutRouting() const;
  void bumpElementIndexAfterInsert() { ++index.element; }

private:
  std::vector<typename ElementTraits<T>::Arrangement> getInArrangements() const;
  std::vector<typename ElementTraits<T>::Arrangement>
  getOutArrangements() const;

  template <typename U, typename V> friend struct ElementImplementation;
  template <typename U> friend struct VstElementImplementation;
  template <typename U> friend struct InMediumCreator;
  template <typename U> friend struct ArrangementGetter;

  typename ElementTraits<T>::Routing routing;
  structure::Index index;
  Merc &merc;

  Media<T> media;

  using VstImpl =
      std::unique_ptr<ElementImplementation<T, structure::VstRunner>>;
  using MercImpl =
      std::unique_ptr<ElementImplementation<T, structure::ChildMercRunner>>;
  std::variant<VstImpl, MercImpl> impl;
};
} // namespace merc::av
