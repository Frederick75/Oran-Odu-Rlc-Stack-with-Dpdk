#include "f1/GtpU.hpp"

namespace odu {

// Minimal GTP-U parser:
// [Flags(1) MsgType(1) Len(2) TEID(4)] then payload
std::optional<GtpuPdu> GtpU::parse(const uint8_t* data, uint16_t len) {
  if (len < 8) return std::nullopt;
  uint8_t msgType = data[1];
  if (msgType != 0xFF /* G-PDU */) return std::nullopt;

  uint16_t gLen = (uint16_t(data[2]) << 8) | data[3];
  if (8 + gLen > len) return std::nullopt;

  uint32_t teid = (uint32_t(data[4]) << 24) | (uint32_t(data[5]) << 16) | (uint32_t(data[6]) << 8) | data[7];
  GtpuPdu p{};
  p.teid = teid;
  p.payload = data + 8;
  p.payload_len = gLen;
  return p;
}

} // namespace odu
