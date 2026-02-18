#pragma once
#include <unordered_map>
#include <shared_mutex>
#include <optional>
#include <memory>

#include "du/UEContext.hpp"

namespace odu {

class UEContextManager {
public:
  void ensure_ue(UeId ue);

  // For demo: configure TEID -> UE/DRB mapping
  void bind_f1u_teid(UeId ue, DrbId drb, uint32_t teid);

  std::optional<std::pair<UeId,DrbId>> lookup_f1u_teid(uint32_t teid) const;

  std::shared_ptr<UeContext> get(UeId ue) const;

private:
  mutable std::shared_mutex mu_;
  std::unordered_map<UeId, std::shared_ptr<UeContext>> ues_;
  std::unordered_map<uint32_t, std::pair<UeId,DrbId>> teid_map_;
};

} // namespace odu
