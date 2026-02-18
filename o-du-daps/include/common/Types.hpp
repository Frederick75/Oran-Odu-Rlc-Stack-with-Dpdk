#pragma once
#include <cstdint>
#include <string>

namespace odu {

using UeId = uint32_t;
using DrbId = uint8_t;

enum class Leg : uint8_t { SOURCE = 0, TARGET = 1 };

struct Endpoint {
  std::string ip;
  uint16_t port{};
};

} // namespace odu
