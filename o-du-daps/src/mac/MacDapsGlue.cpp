#include "mac/MacDapsGlue.hpp"
#include "mac/MacScheduler.hpp"
#include "common/Log.hpp"

namespace odu {

void MacDapsGlue::on_daps_state_change(UeId ue, DrbId drb, bool targetActive) {
  Log::info("MAC glue: daps_state_change ue=%u drb=%u targetActive=%d", ue, (unsigned)drb, (int)targetActive);
  sched_->set_target_active(ue, drb, targetActive);
}

void MacDapsGlue::on_switch_to_target(UeId ue, DrbId drb) {
  Log::info("MAC glue: switch_to_target ue=%u drb=%u", ue, (unsigned)drb);
  sched_->switch_to_target(ue, drb);
}

} // namespace odu
