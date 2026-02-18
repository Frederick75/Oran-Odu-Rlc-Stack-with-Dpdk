#include "du/DapsManager.hpp"
#include "du/UEContextManager.hpp"
#include "rlc/RlcBearer.hpp"
#include "mac/MacDapsGlue.hpp"
#include "common/Log.hpp"

namespace odu {

void DapsManager::start_daps(UeId ue, DrbId drb) {
  auto ctx = uecm_->get(ue);
  if (!ctx) return;
  std::lock_guard<std::mutex> g(ctx->mu);
  auto& st = ctx->daps[drb];
  st.active_source = true;
  st.active_target = true;   // DAPS window => both active
  st.switched_to_target = false;
  Log::info("DAPS START: ue=%u drb=%u (source+target active)", ue, (unsigned)drb);

  mac_->on_daps_state_change(ue, drb, /*targetActive*/true);
}

void DapsManager::switch_to_target(UeId ue, DrbId drb) {
  auto ctx = uecm_->get(ue);
  if (!ctx) return;
  std::lock_guard<std::mutex> g(ctx->mu);
  auto& st = ctx->daps[drb];
  if (!st.active_target) {
    Log::warn("DAPS switch requested but target not active: ue=%u drb=%u", ue, (unsigned)drb);
    return;
  }
  st.switched_to_target = true;
  Log::info("DAPS SWITCH->TARGET: ue=%u drb=%u", ue, (unsigned)drb);

  mac_->on_switch_to_target(ue, drb);
}

void DapsManager::end_daps(UeId ue, DrbId drb) {
  auto ctx = uecm_->get(ue);
  if (!ctx) return;
  std::lock_guard<std::mutex> g(ctx->mu);
  auto& st = ctx->daps[drb];
  st.active_source = false;
  st.active_target = true;
  st.switched_to_target = true;
  Log::info("DAPS END: ue=%u drb=%u (target only)", ue, (unsigned)drb);

  mac_->on_daps_state_change(ue, drb, /*targetActive*/true);
  rlc_->flush_source_buffers(ue, drb);
}

void DapsManager::on_f1u_downlink_pdu(UeId ue, DrbId drb, const uint8_t* p, uint16_t n) {
  auto ctx = uecm_->get(ue);
  if (!ctx) return;

  bool src=false, tgt=false, switched=false;
  {
    std::lock_guard<std::mutex> g(ctx->mu);
    auto it = ctx->daps.find(drb);
    if (it != ctx->daps.end()) {
      src = it->second.active_source;
      tgt = it->second.active_target;
      switched = it->second.switched_to_target;
    } else {
      src = true; tgt = false; switched = false;
    }
  }

  // Policy:
  // - During DAPS (src+tgt active): duplicate to both legs for zero interruption (simplified)
  // - After switch/end: prefer TARGET only
  if (tgt && (switched || !src)) {
    rlc_->rx_downlink_sdu(ue, drb, Leg::TARGET, p, n);
  } else if (src && tgt && !switched) {
    rlc_->rx_downlink_sdu(ue, drb, Leg::SOURCE, p, n);
    rlc_->rx_downlink_sdu(ue, drb, Leg::TARGET, p, n);
  } else {
    rlc_->rx_downlink_sdu(ue, drb, Leg::SOURCE, p, n);
  }
}

} // namespace odu
