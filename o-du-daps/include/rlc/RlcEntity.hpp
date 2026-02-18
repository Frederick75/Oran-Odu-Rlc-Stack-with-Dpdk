#pragma once
#include <cstdint>
#include <deque>
#include <vector>
#include <mutex>

namespace odu {

// Simplified RLC entity: buffer + simple segmentation.
// Real AM/UM would include SN/HDR, reordering, retransmissions, etc.
class RlcEntity {
public:
  void rx_sdu(const uint8_t* p, uint16_t n);
  std::vector<uint8_t> pop_for_mac_pdu(size_t max_bytes);
  void flush();

private:
  std::mutex mu_;
  uint32_t sn_{0};
  std::deque<std::vector<uint8_t>> tx_q_;
};

} // namespace odu
