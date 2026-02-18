#include "mac/MacScheduler.hpp"
#include "rlc/RlcBearer.hpp"
#include "du/UEContextManager.hpp"
#include "common/Log.hpp"
#include <chrono>

namespace odu {

bool MacScheduler::start(RlcBearer* rlc, UEContextManager* uecm) {
  rlc_ = rlc; uecm_ = uecm;
  run_ = true;
  th_ = std::thread([this]{ tti_loop(); });
  Log::info("MAC scheduler started");
  return true;
}

void MacScheduler::stop() {
  run_ = false;
  if (th_.joinable()) th_.join();
  Log::info("MAC scheduler stopped");
}

void MacScheduler::set_target_active(UeId ue, DrbId, bool active) {
  std::lock_guard<std::mutex> g(mu_);
  auto& st = ue_state_[ue];
  st.schedule_target = active;
}

void MacScheduler::switch_to_target(UeId ue, DrbId) {
  std::lock_guard<std::mutex> g(mu_);
  auto& st = ue_state_[ue];
  st.schedule_source = false;
  st.schedule_target = true;
}

void MacScheduler::tti_loop() {
  while (run_) {
    std::this_thread::sleep_for(std::chrono::milliseconds(1)); // demo 1ms TTI

    std::unordered_map<UeId, UeMacState> snapshot;
    {
      std::lock_guard<std::mutex> g(mu_);
      snapshot = ue_state_;
    }

    for (auto& [ue, st] : snapshot) {
      constexpr DrbId DRB = 1;
      if (st.schedule_source) {
        auto pdu = rlc_->build_mac_pdu(ue, DRB, Leg::SOURCE, 1200);
        if (!pdu.empty()) {
          Log::debug("MAC(TTI): ue=%u SOURCE bytes=%zu", ue, pdu.size());
        }
      }
      if (st.schedule_target) {
        auto pdu = rlc_->build_mac_pdu(ue, DRB, Leg::TARGET, 1200);
        if (!pdu.empty()) {
          Log::debug("MAC(TTI): ue=%u TARGET bytes=%zu", ue, pdu.size());
        }
      }
    }
  }
}

} // namespace odu
