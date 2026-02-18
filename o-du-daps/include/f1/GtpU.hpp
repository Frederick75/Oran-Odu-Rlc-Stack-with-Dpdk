#pragma once
#include <cstdint>
#include <optional>

namespace odu {

struct GtpuPdu {
  uint32_t teid{};
  const uint8_t* payload{nullptr};
  uint16_t payload_len{};
};

class GtpU {
public:
  static std::optional<GtpuPdu> parse(const uint8_t* data, uint16_t len);
};

} // namespace odu
