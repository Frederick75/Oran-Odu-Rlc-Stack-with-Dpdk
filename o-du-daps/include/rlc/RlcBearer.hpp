#pragma once
#include <unordered_map>
#include <mutex>
#include "common/Types.hpp"
#include "rlc/RlcEntity.hpp"

namespace odu {

struct BearerKey {
  UeId ue;
  DrbId drb;
  Leg leg;
  bool operator==(const BearerKey& o) const { return ue==o.ue && drb==o.drb && leg==o.leg; }
};

struct BearerKeyHash {
  size_t operator()(const BearerKey& k) const noexcept {
    return (size_t(k.ue) << 16) ^ (size_t(k.drb) << 8) ^ size_t(k.leg);
  }
};

class RlcBearer {
public:
  void rx_downlink_sdu(UeId ue, DrbId drb, Leg leg, const uint8_t* p, uint16_t n);
  std::vector<uint8_t> build_mac_pdu(UeId ue, DrbId drb, Leg leg, size_t max_bytes);
  void flush_source_buffers(UeId ue, DrbId drb);

private:
  RlcEntity& get_or_create(UeId ue, DrbId drb, Leg leg);

  std::mutex mu_;
  std::unordered_map<BearerKey, RlcEntity, BearerKeyHash> ents_;
};

} // namespace odu
