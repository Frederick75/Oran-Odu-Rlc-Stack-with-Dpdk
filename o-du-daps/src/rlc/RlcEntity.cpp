#include "rlc/RlcEntity.hpp"

namespace odu {

void RlcEntity::rx_sdu(const uint8_t* p, uint16_t n) {
  std::lock_guard<std::mutex> g(mu_);
  std::vector<uint8_t> v(p, p + n);
  tx_q_.push_back(std::move(v));
  sn_++;
}

std::vector<uint8_t> RlcEntity::pop_for_mac_pdu(size_t max_bytes) {
  std::lock_guard<std::mutex> g(mu_);
  if (tx_q_.empty()) return {};
  auto& front = tx_q_.front();
  if (front.size() <= max_bytes) {
    auto out = std::move(front);
    tx_q_.pop_front();
    return out;
  }
  std::vector<uint8_t> out(front.begin(), front.begin() + max_bytes);
  front.erase(front.begin(), front.begin() + max_bytes);
  return out;
}

void RlcEntity::flush() {
  std::lock_guard<std::mutex> g(mu_);
  tx_q_.clear();
}

} // namespace odu
