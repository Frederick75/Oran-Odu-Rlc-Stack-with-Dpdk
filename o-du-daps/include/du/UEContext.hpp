#pragma once
#include <unordered_map>
#include <mutex>
#include "common/Types.hpp"

namespace odu {

struct DapsBearerState {
  bool active_source{true};
  bool active_target{false};
  bool switched_to_target{false};
};

struct UeContext {
  UeId ue{};
  std::mutex mu;

  // TEID mapping (downlink) -> DRB
  std::unordered_map<uint32_t, DrbId> teid_to_drb;

  // Per DRB DAPS state
  std::unordered_map<DrbId, DapsBearerState> daps;
};

} // namespace odu
