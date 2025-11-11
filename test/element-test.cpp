#include "illumia/space.h"
#include <gtest/gtest.h>

TEST(Element, BuzzTriangle) {
  std::vector<std::filesystem::path> vstDirs{TEST_CMAKE_BINARY_DIR "/vst3"};
  std::filesystem::path cachePath{TEST_CMAKE_BINARY_DIR "/space-cache.csv"};
  illumia::Space space{vstDirs, cachePath};
  auto buzz{space.getComponent("8F035F10DE6A3FC5A5A761DC12E2BDEF")};
  auto triangle{space.getComponent("CCF29ACC1C1A332899C0953DF892B244")};
}
