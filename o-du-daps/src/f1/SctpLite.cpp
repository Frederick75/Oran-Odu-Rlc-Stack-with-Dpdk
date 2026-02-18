#include "f1/SctpLite.hpp"

namespace odu {

// Demo wire-format: [type(1) ue(4) drb(1)]
std::optional<CtrlMsg> SctpLite::parse(const uint8_t* data, uint16_t len) {
  if (len < 6) return std::nullopt;
  CtrlMsg m{};
  m.type = static_cast<CtrlMsgType>(data[0]);
  m.ue = (uint32_t(data[1]) << 24) | (uint32_t(data[2]) << 16) | (uint32_t(data[3]) << 8) | data[4];
  m.drb = data[5];
  return m;
}

} // namespace odu
