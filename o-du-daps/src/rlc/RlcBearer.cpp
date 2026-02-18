#include "rlc/RlcBearer.hpp"

namespace odu {

RlcEntity& RlcBearer::get_or_create(UeId ue, DrbId drb, Leg leg) {
  BearerKey k{ue, drb, leg};
  auto it = ents_.find(k);
  if (it == ents_.end()) {
    it = ents_.emplace(k, RlcEntity{}).first;
  }
  return it->second;
}

void RlcBearer::rx_downlink_sdu(UeId ue, DrbId drb, Leg leg, const uint8_t* p, uint16_t n) {
  std::lock_guard<std::mutex> g(mu_);
  get_or_create(ue, drb, leg).rx_sdu(p, n);
}

std::vector<uint8_t> RlcBearer::build_mac_pdu(UeId ue, DrbId drb, Leg leg, size_t max_bytes) {
  std::lock_guard<std::mutex> g(mu_);
  auto& e = get_or_create(ue, drb, leg);
  return e.pop_for_mac_pdu(max_bytes);
}

void RlcBearer::flush_source_buffers(UeId ue, DrbId drb) {
  std::lock_guard<std::mutex> g(mu_);
  BearerKey k{ue, drb, Leg::SOURCE};
  auto it = ents_.find(k);
  if (it != ents_.end()) it->second.flush();
}

} // namespace odu
