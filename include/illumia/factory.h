#pragma once

#include <memory>

namespace illumia {
struct Factory {
  Factory();
  ~Factory();
  std::unique_ptr<struct Graphics> graphics;
};
} // namespace illumia
