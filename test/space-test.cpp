#include "illumia/space.h"
#include <filesystem>
#include <gtest/gtest.h>

TEST(Space, CanMakeFromFoldersWithCache) {
  std::vector<std::filesystem::path> vstDirs{TEST_CMAKE_BINARY_DIR "/vst3"};
  std::filesystem::path cachePath{TEST_CMAKE_BINARY_DIR "/space-cache.csv"};
  if (std::filesystem::is_regular_file(cachePath))
    std::filesystem::remove(cachePath);
  {
    illumia::Space space{vstDirs, cachePath};
    auto buzz{space.getComponent("8F035F10DE6A3FC5A5A761DC12E2BDEF")};
    ASSERT_TRUE(buzz);
  }
  {
    illumia::Space space{vstDirs, cachePath};
    auto buzz{space.getComponent("8F035F10DE6A3FC5A5A761DC12E2BDEF")};
    ASSERT_TRUE(buzz);
  }
}
