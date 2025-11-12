#include "illumia/space.h"
#include <gtest/gtest.h>
#include <pluginterfaces/vst/vstspeaker.h>

TEST(Element, BuzzTriangle) {
  std::vector<std::filesystem::path> vstDirs{TEST_CMAKE_BINARY_DIR "/vst3"};
  std::filesystem::path cachePath{TEST_CMAKE_BINARY_DIR "/space-cache.csv"};
  illumia::Space space{vstDirs, cachePath};

  auto buzzComponent{space.getComponent("8F035F10DE6A3FC5A5A761DC12E2BDEF")};
  illumia::Link buzzLink{std::move(buzzComponent)};
  buzzLink.setArrangement({}, {Steinberg::Vst::SpeakerArr::kStereo});
  buzzLink.activate();

  illumia::Revolver<illumia::Samples> audioRevolver{};
  illumia::RealtimeAudio{audioRevolver};

  illumia::TimeKeeper timeKeeper;
  for (int i{0}; i < 2000; ++i) {
    buzzLink.run(timeKeeper, audioRevolver.lock());
    audioRevolver.load();
  }
}
