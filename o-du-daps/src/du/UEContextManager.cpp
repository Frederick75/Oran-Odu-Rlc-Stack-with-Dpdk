#include "du/UEContextManager.hpp"
#include "common/Log.hpp"

namespace odu {

void UEContextManager::ensure_ue(UeId ue) {
  std::unique_lock lk(mu_);
  if (ues_.count(ue)) return;
  auto ctx = std::make_shared<UeContext>();
  ctx->ue = ue;
  ues_[ue] = ctx;
  Log::info("UE context created: ue=%u", ue);
}

void UEContextManager::bind_f1u_teid(UeId ue, DrbId drb, uint32_t teid) {
  ensure_ue(ue);
  {
    std::unique_lock lk(mu_);
    teid_map_[teid] = {ue, drb};
  }
  auto ctx = get(ue);
  if (ctx) {
    std::lock_guard<std::mutex> g(ctx->mu);
    ctx->teid_to_drb[teid] = drb;
  }
  Log::info("Bound TEID: teid=%u -> ue=%u drb=%u", teid, ue, (unsigned)drb);
}

std::optional<std::pair<UeId,DrbId>> UEContextManager::lookup_f1u_teid(uint32_t teid) const {
  std::shared_lock lk(mu_);
  auto it = teid_map_.find(teid);
  if (it == teid_map_.end()) return std::nullopt;
  return it->second;
}

std::shared_ptr<UeContext> UEContextManager::get(UeId ue) const {
  std::shared_lock lk(mu_);
  auto it = ues_.find(ue);
  if (it == ues_.end()) return {};
  return it->second;
}

} // namespace odu
